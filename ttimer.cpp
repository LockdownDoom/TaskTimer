#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

#pragma pack(push,1)
struct Timer{
	int seconds;
	bool running;
	time_t start_time;
	char name[20];
};
#pragma pack(pop)

vector<Timer> timers;

void read_timers(){
	ifstream input;
	input.open("timers.bin", ios::binary);
	char char_buf;
	while (input >> char_buf){
		input.putback(char_buf);
		Timer new_timer;
		input.read(reinterpret_cast<char*>(&new_timer),sizeof(Timer));
		if (strlen(new_timer.name)>0){
			timers.push_back(new_timer);
		}
	}
	input.close();
}

void createTimer(const char * name){
	bool duplicate=false;
	for (vector<Timer>::iterator it = timers.begin(); it != timers.end(); it++){
		Timer timer = *it;
		if (0 == strcmp(timer.name, name)){
			duplicate=true;
		}
	}
	if (!duplicate){
		Timer new_timer;
		strcpy(new_timer.name, name);
		new_timer.running=false;
		new_timer.seconds=0;
		timers.push_back(new_timer);
	}else{
		cout << "There is already a timer for the task \"" << name << "\"." << endl;
	}
}

void editTimer(const char * name, bool running, bool del){
	for (vector<Timer>::iterator it = timers.begin(); it != timers.end(); it++){
		Timer timer = *it;
		if (0 == strcmp(name,timer.name)){
			timer.running=running;
			if (running){
				timer.start_time=time(NULL);
			}else{
				timer.seconds+=(int) difftime(time(NULL), timer.start_time);
			}
			if (del){
				timers.erase(it);
				break;
			}else{
				timers.at(it-timers.begin()) = timer;
			}
		}
	}
}

void write_timers(){
	ofstream output;
	output.open("timers.bin", ios::binary);
	for (vector<Timer>::iterator it = timers.begin(); it != timers.end(); it++){
		Timer write_timer = *it;
		output.write(reinterpret_cast<char*>(&write_timer),sizeof(Timer));
	}
	output.close();
}

string duration(Timer timer){
	int seconds;
	if (timer.running){
		seconds = timer.seconds + difftime(time(NULL), timer.start_time);
	}else{
		seconds = timer.seconds;
	}
	int hours = seconds/3600;
	int minutes=(seconds-(3600*hours))/60;
	string hr;
	if (to_string(hours).length() == 1){
		hr = "0"+to_string(hours);
	}else{
		hr=to_string(hours);
	}
	string min;
	if (to_string(minutes).length() == 1){
		min = "0"+to_string(minutes);
	}else{
		min=to_string(minutes);
	}
	return hr+"h "+min+"m";
}

void listTimers(){
	cout << "NAME                "
		<< "TIME                "
		<< "STATUS" << endl;
	for (vector<Timer>::iterator it = timers.begin(); it != timers.end(); it++){
		Timer current_timer = *it;
		cout << current_timer.name << flush;
		for (int i=0;i<20-strlen(current_timer.name);i++){
			cout << " " << flush; 
		}
		cout << duration(current_timer) << "             " << flush;
		cout << (current_timer.running ? "RUNNING" : "STOPPED") << endl;
	}
}

int main(int argc, const char * argv[]){
	read_timers();
	enum args{
		list_t,
		new_t,
		start_t,
		stop_t,
		del_t
	};
	args op;
	if (argv[1][1] == 'n'){
		op = new_t;
	}
	if (argv[1][1] == 'l'){
		op = list_t;
	}
	if (argv[1][1] == 's'){
		op = start_t;
	}
	if (argv[1][1] == 'p'){
		op = stop_t;
	}
	if (argv[1][1] == 'd'){
		op = del_t;
	}
	switch(op){
		case new_t:
			createTimer(argv[2]);
			break;
		case list_t:
			listTimers();
			break;
		case start_t:
			editTimer(argv[2],true,false);
			break;
		case stop_t:
			editTimer(argv[2],false,false);
			break;
		case del_t:
			editTimer(argv[2],false,true);
			break;
	}
	if (op != list_t){
		write_timers();
	}
	return 0;
}
