$dirs = @("include", "src")
$patterns = "*.cpp","*.h","*.hpp","*.c","*.cxx","*.hxx","*.cmake","*.make","*.json"

$files = Get-ChildItem -Path $dirs -Recurse -Include $patterns -File

foreach ($file in $files) {
    Write-Output "$($file.FullName):"
    Get-Content $file.FullName
    Write-Output ""
}
