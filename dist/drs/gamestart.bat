@echo off

cd /d %~dp0

if not exist dev\nvram mkdir dev\nvram
if not exist dev\nvram\ea3-config.xml copy prop\ea3-config.xml dev\nvram\ea3-config.xml
if not exist dev\nvram\coin.xml copy prop\coin.xml dev\nvram\coin.xml
if not exist dev\nvram\eacoin.xml copy prop\eacoin.xml dev\nvram\eacoin.xml
if not exist dev\raw mkdir dev\raw

.\launcher.exe -H 402653184 .\superstep.dll %*
