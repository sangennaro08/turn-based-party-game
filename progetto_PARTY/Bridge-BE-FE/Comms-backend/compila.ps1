# Spostati nella cartella corrente del file
cd $PSScriptRoot

# Lancia il compilatore con i percorsi corretti di pybind11 e Python
cl.exe /Zi /EHsc /LD /std:c++latest /I ".\pybind11\include" /I "C:\Users\Studente\AppData\Local\\Programs\Python\Python314\include" .\Comunication.cpp /Fe:Comunication.pyd /link /LIBPATH:"C:\Users\Studente\AppData\Local\Programs\Python\Python314\libs" python314.lib