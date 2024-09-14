#include <iostream>
#include "lib/archive.h"
#include <bitset>
#include <cstring>
#include <vector>

int main(int argc, char* argv[]) {
    std::vector<Archive> all_archives;
    int i = 1;
    while (i < argc) {
        //create
        Archive archive;
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--create") == 0) {
            i++;
            if (strcmp(argv[i], "-f") == 0) {
                i++;
                archive.archive_path = argv[i];
                i++;
                while (i < argc && argv[i][0] != '-') {
                    archive.IncludeFile(argv[i]);
                    i++;
                }
            } else {
                std::string archive_path;
                int j = 0;
                while (argv[i][j] != '=') {
                    j++;
                }
                j++;
                while (j != '\0') {
                    archive_path += argv[i][j];
                    j++;
                }
                archive.archive_path = archive_path;
                i++;
                while (i < argc && argv[i][0] != '-') {
                    archive.IncludeFile(argv[i]);
                    i++;
                }
            }
            all_archives.push_back(archive);
        }
            //delete -команда , архив , файл из архива
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--delete") == 0) {
            i++;
            std::string archive_name = argv[i];
            for (int j = 0; j < all_archives.size(); j++) {
                if (archive_name == all_archives[j].archive_path) {
                    i++;
                    all_archives[j].DeleteFile(argv[i]);
                    break;
                }
            }
        }
            //extract -команда , архив , файлы(если есть)
        else if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--extract") == 0) {
            i++;
            std::string archive_name = argv[i];
            for (int j = 0; j < all_archives.size(); j++) {
                if (archive_name == all_archives[j].archive_path) {
                    i++;
                    if (argv[i][0] == '-') {
                        for (int k = 0; k < all_archives[j].files.size(); k++) {
                            all_archives[j].GetFile(all_archives[j].files[k].file_name);
                        }
                    } else {
                        all_archives[j].GetFile(argv[i]);
                    }
                    break;
                }
            }
        }
            //append -команда , архив , файл
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--append") == 0) {
            i++;
            std::string archive_name = argv[i];
            for (int j = 0; j < all_archives.size(); j++) {
                if (archive_name == all_archives[j].archive_path) {
                    i++;
                    all_archives[j].IncludeFile(argv[i]);
                    break;
                }
            }
        }
            // -l --list - команда , архив
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            i++;
            std::string archive_name = argv[i];
            for (int j = 0; j < all_archives.size(); j++) {
                if (archive_name == all_archives[j].archive_path) {
                    all_archives[j].Show_List();
                    break;
                }
            }
        }
        //-A --concatenate
        else if (strcmp(argv[i], "-A") == 0 || strcmp(argv[i], "--concatenate") == 0){
            i++;
            std::string archive_name1 = argv[i];
            i++;
            std::string archive_name2 = argv[i];
            int it1 = 0 , it2 = 0;

            for (int j = 0; j < all_archives.size(); j++) {
                if (archive_name1 == all_archives[j].archive_path) {
                    it1 = j;
                }
                if (archive_name2 == all_archives[j].archive_path) {
                    it2 = j;
                }
            }
            i++;
            if (strcmp(argv[i] , "-f") == 0){
                Archive local_archive;
                i++;
                local_archive.archive_path = argv[i];
                local_archive.Make_One_Archive(all_archives[it1],all_archives[it2]);
                all_archives.push_back(local_archive);
            }
            else{
                Archive local_archive;
                std::string archive_path;
                int j = 0;
                while (argv[i][j] != '=') {
                    j++;
                }
                j++;
                while (j != '\0') {
                    archive_path += argv[i][j];
                    j++;
                }
                local_archive.archive_path = archive_path;
                local_archive.Make_One_Archive(all_archives[it1],all_archives[it2]);
                all_archives.push_back(local_archive);
            }
        }
        i++;
    }

}