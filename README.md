# EzBackup
An easy way to create backups of your directories.

# Instalation

there are two options:
1. Download the repro and use it with Visual Studio
   (You can also also delet "build" and confidure with cmake)
3. Build without "AUTOSTART" defined
4. Build with "AUTOSTART" defined
5. grab the exe from /build/{Release/Debug}/

# Usage

Open "EzBackupUI.exe" (or the one build without "AUTOSTART" defined) and add backup paths like so:
- Press "Add Backup" button
- A window called "Add Backup" should open
- There You can spezifie:
  - Name (self explanatory)
  - original Path (path to the directory you want to backup)
  - backup Path (path to were the backup should be saved)
- Press the "Add" button
- Press the "Save" button

- The added backup should be shown under --------backups---------
  - there you can press it and change the Name and Paths 

- to backup, press Backup All



# Credits

https://github.com/onurae/gui-app-template.git (used as tempelate)
