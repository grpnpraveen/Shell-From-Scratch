
#include <iostream>
#include<string>
#include <filesystem>
#include<vector>
#include<unordered_map>
//#include<cstdlib>
using namespace std;

//Struct 

struct PATH_DATA
{
	bool query_found;
	string path;
};

//Functions 
void strip(string& str)
{
	//Function vars
	if (str.empty()) //Empty?
	{
		return;
	}
	int index_start = 0;
	int index_end= str.length() - 1;
	//Function logic
	while(::isspace(static_cast<unsigned char>(str.at(index_start))))
	{
		index_start++;
	}
	while (::isspace(static_cast<unsigned char>(str.at(index_end))))
	{
		index_end--;
	}
	str = str.substr(index_start, index_end-index_start+1);
}

string split(string str, string delimiter, int index, vector<string>& separated_items)
{

	int i = 0;
	int pos = str.find(delimiter);
	string res = str.substr(i, pos);
	while (true)
	{
		separated_items.push_back(res);

		if (index == i)
		{
			break;
		}

		res = str.substr(pos + 1, str.find(delimiter, pos + 1) - pos - 1);
		if (str.find(delimiter, pos + 1) == string::npos)
		{
			res = str.substr(pos + 1, -1);
			separated_items.push_back(res);
			break;
		}
		else {
			pos = str.find(delimiter, pos + 1);
		}

		i++;
	}

	return res;
}

struct PATH_DATA is_exec_in_path(string query) 
{
	struct PATH_DATA path_data;
	path_data.query_found = false; 

	string PATH = getenv("PATH");
	string delimiter = ":";
	string delimiter2 = "/";
	#ifdef _WIN32
		delimiter = ";";
		delimiter2 = "\\";
	#endif

	vector<string> separated_item_lst,  temp_separated_item; ;
	split(PATH, delimiter, -1, separated_item_lst); 

	for (auto each_path : separated_item_lst) 
	{
		for (auto each_file : filesystem::directory_iterator(each_path))
		{

			temp_separated_item.clear();
			string tmp_exec = split(each_file.path().string(), delimiter2, -1, temp_separated_item); 
			//cout << tmp_exec << "\n"; 

			if (query == tmp_exec)
			{
				path_data.query_found = !path_data.query_found;
				path_data.path = each_file.path().string();

				break;
			}
			
		}
		if (path_data.query_found) 
		{
			break;
		}
	}

	return path_data; 
}

void command_error(string input)
{
	cout << input << ": " << "command not found\n";
}

void query_type(string query, struct PATH_DATA path_data)
{
	if (!path_data.query_found) 
	{
		cout << query << ": " << "not found\n";
		return;
	}
	cout << query << " is " << path_data.path << "\n";

}

// Main
int main() {
  
  //Vars 
  unordered_map<string, int> cmds;
  //Commands
  string exit = "exit";
  string echo = "echo";
  string type = "type";
  string cd = "cd";
  string empty = "";
  //Commands fun 
  cmds[exit] = 1;
  cmds[empty] = 1;
  cmds[echo] = 1;
  cmds[type] = 1;
  cmds[cd] = 1;
  //Loop control
  int run_loop = 1;
  string input; 
  string command; 
  string query;  
  vector<string> separated_item_lst; 
  struct PATH_DATA path_data;
  path_data.query_found = false; 
  //Main logic
  while (run_loop) {


	  //Code
	  cout << unitbuf;
	  cerr << unitbuf;

	  // Shell starts
	  cout << "$ ";

	

	  getline(cin, input);

	  strip(input); //Strip input

	  command = split(input, " ", 0, separated_item_lst); //Split based on delimiter


	  if (cmds[command] == 0)
	  {
		 
		  path_data = is_exec_in_path(command); 

		  if (!path_data.query_found) 
		  {
			  command_error(command);
		  }
		  else {

			  //External code running
			  if (input.find(" ") != string::npos)
			  {
				  system((path_data.path + input.substr(input.find(" "))).c_str()); 
			  }
			  else {
				  system((path_data.path).c_str());
			  }
			 
		  }
		
	  }
	  else if (command == exit)
	  {
		  separated_item_lst.clear(); //Freeing memory 
		  if ( split(input, " ", 1, separated_item_lst) == "0" )
		  {
			  run_loop = 0;
		  }
		  else {

			  command_error(input);
		  }
	  }
	  else if (command == echo)
	  {
		  cout << input.substr(input.find(" ")+1, input.length()- input.find(" ") + 2) << "\n";
	  }
	  else if (command == type)
	  {
		  path_data.query_found = false; 
		  query = input.substr(input.find(" ") + 1, input.length() - input.find(" ") + 2);

		  if (cmds[query] == 1)
		  {
			  cout << query << " is a shell builtin"<<"\n";
			  path_data.query_found = !path_data.query_found;  
		  }
		  else
		  {
			  path_data  = is_exec_in_path(query);
		
			  query_type(query, path_data);
			  
		  }
	  }
	  else if (command == cd)
	  {
		  string HOME = getenv("HOME");
		  if (input.find(" ") != string::npos)
		  {
			  string togo_path = input.substr(input.find(" ") + 1);
			  if (std::filesystem::exists(togo_path))
			  {
				  std::filesystem::current_path(togo_path);
			  }
			  else if (togo_path == "~")
			  {
				  std::filesystem::current_path(HOME); 
			  }
			  else std::cout << togo_path << ": No such file or directory" << std::endl;
		  }
		  else {
			  std::filesystem::current_path(HOME);
		  }
	  }
	 
  };

}
