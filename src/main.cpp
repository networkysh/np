#include <tclap/CmdLine.h>
#include <string>
#include <algorithm>

bool silent_mode = false;
std::string project_name = std::string("");
std::string template_file = std::string("");
std::string template_make = std::string("");

bool does_folder_exist(std::string folder) { return system(("test -d " + folder).c_str()); }

bool does_file_exist(std::string file) { return !system(("test -f " + file).c_str()); }

std::string concat_folders(std::string parent, std::string child) {
  return parent + "/" + child;
}

void sed_file(std::string file, std::string pattern) {
  system(((("sed -i '' " + pattern ) + " ") + file).c_str());
}

// thank you, T.E.D (Stack Overflow)
void inline replace_all(std::string &str, const std::string& from, const std::string& to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}

void make_folder(std::string parent, std::string name) {
  if(!does_folder_exist(parent))
    if(does_folder_exist(concat_folders(parent, name))) {
      if(!silent_mode) std::cout << "Successfully created folder \"" << concat_folders(parent, name) << "\"." << std::endl;
      system(("mkdir " + concat_folders(parent, name)).c_str());
    } else
      std::cerr << "Child folder \"" << name << "\" already exists." << std::endl;
  else
    std::cerr << "Parent file \"" << parent << "\" does not exist."  << std::endl;
}

void touch_file(std::string folder, std::string name, std::string type) {
  system(("touch " + ((folder + "/") + (name + type))).c_str());
  if(!silent_mode) std::cout << "Created file \"" << name << "." << type << "\"." << std::endl;
}

void copy_file(std::string from, std::string to) {
  system(((("cp " + from) + (" " + to))).c_str());
  if(!silent_mode) std::cout << "Copied template file from \"" << from << "\" to \"" << to << "\"." << std::endl;
}

void make_project(std::string directory, std::string name) {
  std::string projectFolder = concat_folders(directory, name);
  make_folder(directory, projectFolder); // make project folder
  make_folder(projectFolder, "src");     //make srcfolder
  std::string mainFile = concat_folders(projectFolder, "src") + "/main.cpp";
  std::string makeFile = projectFolder + "/makefile";
  if(!does_file_exist(makeFile))
    if(!does_file_exist(template_make))
      touch_file(projectFolder, "makefile", "");
    else {
      copy_file(template_make, makeFile);
      sed_file(makeFile, ("\"s/##PN##/" + project_name) + "/\"");
    }
  else
    std::cout << "Makefile \"" << makeFile << "\" already exists." << std::endl;
  if(!does_file_exist(mainFile))
    if(!does_file_exist(template_file))
      touch_file(concat_folders(projectFolder, "src"), "main", ".cpp");
    else {
      copy_file(template_file, mainFile);
      sed_file(mainFile, ("\"s/##PN##/" + project_name) + "/\"");
    }
  else
    std::cout << "Main file \"" << mainFile << "\" already exists." << std::endl;
}

int main(int argc, char** argv) {
  try {
    TCLAP::CmdLine cmd("np - simple cpp project maker\n\tmade by haze, (c) 2016.", ' ', "0.01a");
    TCLAP::ValueArg<std::string> pathArg("d", "directory", "Directory to create project folder", false, ".", "string");
    TCLAP::ValueArg<std::string> templateFileArg("m", "maintemplate", "Template file (main.cpp)", false, "/etc/np/temp_main.cpp", "string");
    TCLAP::ValueArg<std::string> templateMakeArg("k", "maketemplate", "Template file (makefile)", false, "/etc/np/temp_makefile", "string");
    TCLAP::SwitchArg quiet("q", "quiet", "Print no output", cmd, false);
    TCLAP::UnlabeledValueArg<std::string> projectName("name", "project name", true, "unnamed project", "string");
    cmd.add(templateFileArg);
    cmd.add(templateMakeArg);
    cmd.add(pathArg);
    cmd.add(projectName);
    cmd.parse(argc, argv);
    silent_mode = quiet.getValue();
    template_file = templateFileArg.getValue();
    template_make = templateMakeArg.getValue();
    char quote = '"';
    std::string name = projectName.getValue();
    if(name.at(0) == quote && name.at(name.size()) == quote) {
      name = name.substr(1, name.size() - 1);
    }
    if(name.find(" ") != std::string::npos)
      replace_all(name, " ", "\\ ");
    project_name = name;
    std::string directory = pathArg.getValue();
    if(!silent_mode && does_file_exist(template_file)) std::cout << "Found template file at \"" << template_file << "\"." << std::endl;
    if(!silent_mode) std::cout << "Creating project \"" << name << "\"...." << std::endl;
    make_project(directory, name);
  } catch(TCLAP::ArgException &e) {
    std::cerr << "argument error found: " << e.error() << " for \"" << e.argId() <<"\"" << std::endl;
  }
  return 0;
}
