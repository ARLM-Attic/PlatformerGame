@set builder=Debug\PlatformerGame.exe -t

rmdir /s /q data
mkdir data

copy data_src\test_tileset.png data\tileset.png
copy data_src\tilemap.txt data\
copy data_src\ui_font.png data\
%builder% --pack-sprites data_src\characters\ data\characters

pause
