echo "" > .\storage\%1.txt
echo "" > .\storage\%2.txt
echo "" > .\storage\%3.txt
del /q .\storage\%1.txt
del /q .\storage\%2.txt
del /q .\storage\%3.txt
echo "" > .\storage\%1.txt
echo "" > .\storage\%2.txt
echo "" > .\storage\%3.txt
python generate.py %4 > storage/%1.txt
.\objects\app.exe -i .\storage\%1.txt -o .\storage\%2.txt -r %5 -conf .\storage\configs.txt -enc
.\objects\app.exe -i .\storage\%2.txt -o .\storage\%3.txt -r %5 -conf .\storage\configs.txt -dec 
type .\storage\%1.txt
type .\storage\%2.txt
type .\storage\%3.txt