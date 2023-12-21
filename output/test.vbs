Set objShell = CreateObject("WScript.Shell")

' Path to the executable
exePath = "main.exe"

' Create a new process without a visible window
objShell.Run Chr(34) & exePath & Chr(34) & " /param1 /param2", 0, False

' Optionally, wait for the process to finish
WScript.Sleep 5000  ' Sleep for 5000 milliseconds (adjust as needed)
