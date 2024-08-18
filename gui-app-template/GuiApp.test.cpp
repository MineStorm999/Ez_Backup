

#include "GuiApp.hpp"

#include <vector>
#include <string>
#include <iostream>

#include <Windows.h>
#include <shobjidl.h> 

#include <filesystem>
#include <fstream>
#include "json.hpp"

namespace fs = std::filesystem;

//#define AUTOSTART
struct BackupData {
    std::string name{"New Backup"};
    std::string originalPath{ "C:/Users" };
    std::string backupPath{ "C:/Users" };
};

class MyApp : public GuiApp
{
public:
    MyApp() : GuiApp("EZBackup") {
        
        Init();
#ifdef AUTOSTART
        timeToClose = true;
#endif // AUTOSTART

    };
    ~MyApp() final = default;

    void Update() override
    {
        MainWindow();
        if (addingBackup) {
            BackupAddWindow();
        }
    }

    // backup
    void DoBackups() {
        for (size_t i = 0; i < data.size(); i++)
        {
            DoBackup(i);
        }
    };
    void DoBackup(uint32_t i) {
        const auto copyOptions = fs::copy_options::update_existing
            | fs::copy_options::recursive;


        fs::path originalPath = data[i].originalPath;
        fs::path backupPath = data[i].backupPath + "/" + data[i].name;


        fs::create_directories(backupPath);
        fs::copy(originalPath, backupPath, copyOptions);
    };



    // Windows
    void BackupAddWindow() {
        
        ImGui::Begin("Add Backup");


        ImGui::InputText("Name ", &tempData.name);

        ImGui::InputText("original path ", &tempData.originalPath);
        ImGui::InputText("backup path ", &tempData.backupPath);
        
        

        if (ImGui::Button("ADD")) {
            data.push_back(tempData);
            addingBackup = false;
        }

        ImGui::End();
    };


    void MainWindow() {
        ImGui::Begin("EZBackup");

        if (ImGui::Button("Close", { 300, 30 })) {
            Save();
            timeToClose = true;
        }

        if (ImGui::Button("BackupAll", {300, 60})) {
            DoBackups();
        }

        if (ImGui::Button("Save", { 300, 60 })) {
            Save();
        }
        if (ImGui::Button("Add Backup", {300, 60})) {
            if (!addingBackup) {
                tempData = {};
                addingBackup = true;
            }
        }

        ImGui::Text("---------------------------------------------Backups---------------------------------------------");
        ImGui::BeginChild("Backups");
        for (size_t i = 0; i < data.size(); i++)
        {
            if (selected == i) {
                std::string s = data[i].name + "------------------------------------------------------------------------------------------------------------------------------------------------------------------------" + std::to_string(i);
                if (ImGui::Button(s.c_str(), { 300, 20 })) {
                    selected = -1;
                }

                ImGui::InputText("name", &data[i].name);

                ImGui::InputText("original path", &data[i].originalPath);

                ImGui::InputText("backup path", &data[i].backupPath);

                if (ImGui::Button("Remove")) {
                    std::cout << "Remove" << i << std::endl;
                    data.erase(data.begin() + i);
                }
                if (ImGui::Button("Backup")) {
                    DoBackup(i);// todo
                }
            }
            else {
                std::string s = data[i].name + "------------------------------------------------------------------------------------------------------------------------------------------------------------------------" + std::to_string(i);
                if (ImGui::Button(s.c_str(), { 300, 20 })) {
                    selected = i;
                }
            }
        }

        ImGui::EndChild();
        ImGui::End();
    };
   

    // init/close
    void Init() {
        Load();
        if (data.size() <= 0) {
            addingBackup = true;
        }
        else {
            DoBackups();
        }
    };

    // save/load
    void Load() {
        fs::path pt = fs::current_path().string() + "/EzBackup/save.json";
        std::cout << "load from file: " << pt.string() << std::endl;
        
        std::ifstream f(pt);
        if (!f.is_open()) {
            std::cout << "no save file" << std::endl;
            return;
        }
        nlohmann::json json = nlohmann::json::parse(f);
        if (json["AName"] != 0xe5bac109) {
            std::cout << "wrong file" << std::endl;
            return;
        }
        std::cout << "Loaded: \n" << std::setw(4) << json << std::endl;
        for (size_t i = 0; i < json["Info"]; i++)
        {
            std::string s = "backupName" + i;
            tempData.name = json[s];

            s = "backupOriginalPt" + i;
            tempData.originalPath = json[s];

            s = "backupBackupPt" + i;
            tempData.backupPath = json[s];
            data.push_back(tempData);
            tempData = {};
        }
    };

    void Save() {
        nlohmann::json json;
        json["AName"] = 0xe5bac109;
        json["Info"] = data.size();
        
        for (size_t i = 0; i < data.size(); i++)
        {
            std::string s = "backupName" + i;
            json[s] = data[i].name;

            s = "backupOriginalPt" + i;
            json[s] = data[i].originalPath;
            
            s = "backupBackupPt" + i;
            json[s] = data[i].backupPath;
        }

        std::cout << std::setw(4) << json << std::endl;

        fs::path pt = fs::current_path().string() + "/save.json";
        fs::create_directories(pt.parent_path());
        std::ofstream output(pt);
        if (output.is_open()) {
            output << json << "\n";
            output.close();
        }
        
    };


    bool OpenExplorer(std::string& sSelectedFile,
                      std::string& sFilePath)
    {
        //  CREATE FILE OBJECT INSTANCE
        HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(f_SysHr))
            return FALSE;

        // CREATE FileOpenDialog OBJECT
        IFileOpenDialog* f_FileSystem;
        f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
        if (FAILED(f_SysHr)) {
            CoUninitialize();
            return FALSE;
        }

        //  SHOW OPEN FILE DIALOG WINDOW
        f_SysHr = f_FileSystem->Show(NULL);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
        IShellItem* f_Files;
        f_SysHr = f_FileSystem->GetResult(&f_Files);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  STORE AND CONVERT THE FILE NAME
        PWSTR f_Path;
        f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
        if (FAILED(f_SysHr)) {
            f_Files->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  FORMAT AND STORE THE FILE PATH
        std::wstring path(f_Path);
        std::string c(path.begin(), path.end());
        sFilePath = c;

        //  FORMAT STRING FOR EXECUTABLE NAME
        const size_t slash = sFilePath.find_last_of("/\\");
        sSelectedFile = sFilePath.substr(slash + 1);

        //  SUCCESS, CLEAN UP
        CoTaskMemFree(f_Path);
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();
        return TRUE;
    }

    bool OpenExplorer(std::string& sFilePath)
    {
        //  CREATE FILE OBJECT INSTANCE
        HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(f_SysHr))
            return FALSE;

        // CREATE FileOpenDialog OBJECT
        IFileOpenDialog* f_FileSystem;
        f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
        if (FAILED(f_SysHr)) {
            CoUninitialize();
            return FALSE;
        }

        //  SHOW OPEN FILE DIALOG WINDOW
        f_SysHr = f_FileSystem->Show(NULL);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  RETRIEVE FILE NAME FROM THE SELECTED ITEM
        IShellItem* f_Files;
        f_SysHr = f_FileSystem->GetResult(&f_Files);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  STORE AND CONVERT THE FILE NAME
        PWSTR f_Path;
        f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
        if (FAILED(f_SysHr)) {
            f_Files->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        //  FORMAT AND STORE THE FILE PATH
        std::wstring path(f_Path);
        std::string c(path.begin(), path.end());
        sFilePath = c;

        
        

        //  SUCCESS, CLEAN UP
        CoTaskMemFree(f_Path);
        f_Files->Release();
        f_FileSystem->Release();
        CoUninitialize();
        return TRUE;
    }
private:
    bool open = true;
    bool redock = false;

    float xs1[1001]{};
    float ys1[1001]{};
    double xs2[20]{};
    double ys2[20]{};


    // data
    std::string savePath {"C:/ProgramData/EzBackup/save.json"};

    BackupData tempData {};
    std::vector<BackupData> data;

    bool addingBackup = false;
    uint32_t selected = -1;
};

int main()
{
    MyApp myApp;
    myApp.Run();
    return 0;
}