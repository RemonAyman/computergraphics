$args = @(
    "-IE:/graphicsproject/src",
    "-std=gnu++17",
    "-fdiagnostics-color=always",
    "-c", "E:/graphicsproject/src/App.cpp",
    "-o", "App.obj"
)
$p = Start-Process -FilePath "E:\Program Files\CLion 2025.3.2\bin\mingw\bin\g++.exe" -ArgumentList $args -NoNewWindow -Wait -PassThru -RedirectStandardError "E:\graphicsproject\err.txt" -RedirectStandardOutput "E:\graphicsproject\out.txt"
Write-Output "ExitCode: $($p.ExitCode)"
