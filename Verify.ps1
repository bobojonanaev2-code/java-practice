param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug'
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# Run from any directory; build artifacts remain under the repository's .build/.
$root = $PSScriptRoot
$msbuildCommand = Get-Command msbuild -ErrorAction SilentlyContinue
if ($msbuildCommand) {
    $msbuildPath = $msbuildCommand.Source
} else {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere)) {
        throw 'Не найден Visual Studio Installer. Установите Visual Studio с рабочей нагрузкой C++.'
    }
    $installations = @(& $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath)
    if ($installations.Count -eq 0) {
        throw 'Не найден MSVC. В Visual Studio Installer установите «Разработка классических приложений на C++».'
    }
    $msbuildPath = Join-Path $installations[0] 'MSBuild\Current\Bin\MSBuild.exe'
}
& $msbuildPath (Join-Path $root 'CppPractice.sln') /m "/p:Configuration=$Configuration" /p:Platform=x64 /nologo
if ($LASTEXITCODE -ne 0) { throw "Сборка не выполнена: код $LASTEXITCODE" }

function Invoke-Case {
    param([string]$Project, [string]$Arguments, [string]$InputText, [string[]]$Expected, [string]$Label)
    $exe = Join-Path $root ".build\x64\$Configuration\$Project\$Project.exe"
    $start = New-Object System.Diagnostics.ProcessStartInfo
    $start.FileName = $exe
    $start.Arguments = $Arguments
    $start.UseShellExecute = $false
    $start.CreateNoWindow = $true
    $start.RedirectStandardInput = $true
    $start.RedirectStandardOutput = $true
    $start.RedirectStandardError = $true
    $start.StandardOutputEncoding = [System.Text.UTF8Encoding]::new($false)
    $start.StandardErrorEncoding = [System.Text.UTF8Encoding]::new($false)
    $process = New-Object System.Diagnostics.Process
    $process.StartInfo = $start
    try {
        [void]$process.Start()
        $stdoutTask = $process.StandardOutput.ReadToEndAsync()
        $stderrTask = $process.StandardError.ReadToEndAsync()
        if ($InputText.Length -gt 0) { $process.StandardInput.Write($InputText) }
        $process.StandardInput.Close()
        if (-not $process.WaitForExit(15000)) {
            $process.Kill()
            throw "Превышено время проверки: $Label"
        }
        $output = $stdoutTask.GetAwaiter().GetResult() + $stderrTask.GetAwaiter().GetResult()
        if ($process.ExitCode -ne 0) { throw "$Label завершился с кодом $($process.ExitCode): $output" }
        foreach ($pattern in $Expected) {
            if ($output -notmatch $pattern) { throw "$Label — не найдено '$pattern': $output" }
        }
        Write-Host "OK: $Label"
    } finally {
        $process.Dispose()
    }
}

foreach ($project in @('task-16-8', 'task-16-9')) {
    Invoke-Case $project '--test' '' @('Все проверки задания') "$project — проверки классов"
    Invoke-Case $project '' '' @('Ввод завершён') "$project — немедленный EOF"
    Invoke-Case $project '' "abc`n0`n" @('Ошибка:') "$project — неверная команда"
    Invoke-Case $project '' "1 extra`n0`n" @('Лишние данные') "$project — лишние данные в команде"
}
Invoke-Case 'task-16-8' '--demo' '' @('Частное P / Q = 1\*x1 - 1', 'Остаток = 1\*x2 \+ 3', 'остаток: true') '16.8 — демонстрация'
Invoke-Case 'task-16-8' '' "1`n2`n4`n1 2 0`n3 0 1`n-2 0 1`n2 0 0`n2`n2`n2`n1 1 0`n1 0 0`n10`n0`n" @('Остаток = 1\*x2 \+ 3', 'остаток: true') '16.8 — ввод контрольного примера'
Invoke-Case 'task-16-8' '' "4`n5`n1`n1 0 1`n6`n7`n8`n9`n0`n" @('Принадлежит: true', 'P = Q: false', '1\*x1\^3') '16.8 — операции меню'
Invoke-Case 'task-16-8' '' "2`n2`n0`n10`n0`n" @('Ошибка: Деление на нулевой многочлен') '16.8 — нулевой делитель'
Invoke-Case 'task-16-8' '' "2`n1`n0`n7`n0`n" @('Несовместимое количество переменных') '16.8 — разные размерности'
Invoke-Case 'task-16-8' '' "1`n2`n1`n1/0 1 0`n3`n0`n" @('Знаменатель', 'P = 1\*x1\^2 \+ 1\*x2 \+ 2') '16.8 — ошибка сохраняет P'
Invoke-Case 'task-16-8' '' "1`n2`n1`n1 -1 0`n0`n" @('Ошибка:') '16.8 — отрицательная степень'
Invoke-Case 'task-16-8' '' "1`n2`n1`n1 1`n0`n" @('Введите все степени') '16.8 — недостающая степень'
Invoke-Case 'task-16-8' '' "1`n2`n1`n" @('Ввод завершён') '16.8 — EOF внутри многочлена'
Invoke-Case 'task-16-9' '--demo' '' @('Площадь: 6', 'Периметр: 12', 'Радиус вписанной окружности: 1', 'Радиус описанной окружности: 2.5', 'прямоугольный') '16.9 — демонстрация'
Invoke-Case 'task-16-9' '' "1`n0 0`n3 0`n0 4`n5`n1`n6`n1`n1`n90`n4`n0`n" @('Площадь: 6', 'Координаты после поворота', 'Треугольники равны: true') '16.9 — ввод и поворот'
Invoke-Case 'task-16-9' '' "2`n0 0`n1 0`n0 1`n5`n2`n4`n0`n" @('равнобедренный', 'прямоугольный', 'Треугольники равны: false') '16.9 — второй треугольник и классификация'
Invoke-Case 'task-16-9' '' "1`n0 0`n1 1`n2 2`n5`n1`n0`n" @('Вырожденный', 'Площадь: 6') '16.9 — ошибка сохраняет треугольник'
Invoke-Case 'task-16-9' '' "1`nnan 0`n0`n" @('Ошибка:') '16.9 — NaN во вводе'
Invoke-Case 'task-16-9' '' "1`n1e309 0`n0`n" @('Ошибка:') '16.9 — переполнение ввода'
Invoke-Case 'task-16-9' '' "1`n0 0`n" @('Ввод завершён') '16.9 — EOF внутри треугольника'
Invoke-Case 'task-16-9' '' "6`n1`n4`n" @('Ввод завершён') '16.9 — EOF перед углом'
Write-Host "Все проверки $Configuration завершены."
