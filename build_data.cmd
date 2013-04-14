@set builder=Debug\PlatformerGame.exe -t

rmdir /s /q data
mkdir data

copy data_src\tileset.png data\
copy data_src\tilemap.txt data\
copy data_src\ui_font.png data\
%builder% --pack-sprites data_src\characters\ data\characters

pause
