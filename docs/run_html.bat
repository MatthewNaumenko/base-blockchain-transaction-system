@echo off
cd html
if exist index.html (
    start  index.html
) else (
    echo Файл index.html не найден в папке html.
    pause
)