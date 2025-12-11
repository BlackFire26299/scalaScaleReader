#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <gtk/gtk.h>

typedef std::string string;
typedef double cents;
typedef double freq;
typedef double interval;
typedef struct {
	double* NoteInterval;
	int size;
} NoteIntervalArray;
	

static void start_scale (GtkWidget *widget, gpointer data)
{
	g_print("Starting scale... \n");
	NoteIntervalArray nia = (NoteIntervalArray)data;
	for(int i = 0; i<nia.size; i++){
		std::cout << nia.NoteInterval[i] << std::endl;
	}
}

static void
activate (GtkApplication *app, gpointer user_data)
{
  	GtkWidget *window;
  	GtkWidget *button;
	
  	window = gtk_application_window_new (app);
  	gtk_window_set_title (GTK_WINDOW (window), "Hello");
  	gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
	
	button = gtk_button_new_with_label ("Start Scale");
  	gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
  	gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
  	g_signal_connect (button, "clicked", G_CALLBACK (start_scale), user_data);
  	gtk_window_set_child (GTK_WINDOW (window), button);

  	gtk_window_present (GTK_WINDOW (window));
}

freq centsToRatio(cents Cents){
    	freq frequencyRatio = pow(2, (Cents/1200));
    	return frequencyRatio;
}

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
    	const auto strBegin = str.find_first_not_of(whitespace);
    	if (strBegin == std::string::npos)
    	    return ""; // no content

    	const auto strEnd = str.find_last_not_of(whitespace);
    	const auto strRange = strEnd - strBegin + 1;

    	return str.substr(strBegin, strRange);
}

std::string reduce(const std::string& str, const std::string& fill = " ", const std::string& whitespace = " \t")
{
    	// trim first
    	auto result = trim(str, whitespace);

    	// replace sub ranges
    	auto beginSpace = result.find_first_of(whitespace);
    	while (beginSpace != std::string::npos)
    	{
    	    	const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
    	    	const auto range = endSpace - beginSpace;
	
		result.replace(beginSpace, range, fill);

        	const auto newStart = beginSpace + fill.length();
        	beginSpace = result.find_first_of(whitespace, newStart);
    	}

    	return result;
}

double* GetScaleData(string& ScaleName, int& NumNotes ){
    	std::ifstream ScalaFile;
    	const char* filename = "./scalaFiles/test.scl";
    	ScalaFile.open(filename);
    	if (!ScalaFile.is_open()) {
    	    	std::cerr << "Error opening file: " << filename << std::endl;

        	// Check for specific error conditions
        	if (ScalaFile.bad()) {
        	    std::cerr << "Fatal error: badbit is set." << std::endl;
        	}
	
		if (ScalaFile.fail()) {
            	// Print a more detailed error message using
            	// strerror
            	std::cerr << "Error details: " << strerror(errno) << std::endl;
        	}
        
        
	}
    	string PeiceTitle;
    
    	string tempString;
    	string commentChar("!");
    	while (std::getline(ScalaFile, tempString)) {
    	    char firstChar = tempString.front();
    	    if (firstChar == '!'){
    	        continue;
    	    }
    	    PeiceTitle = tempString;
     	   break;
    	}	
    
    	while (std::getline(ScalaFile, tempString)) {
    	    char firstChar = tempString.front();
    	    if (firstChar == '!'){
    	        continue;
    	    }
    	    NumNotes = std::stoi(reduce(tempString));
    	    break;
    	}
    	void *NI = malloc(sizeof(double) * NumNotes);
    	double *NoteIntervals = (double *)NI;
    	int i = 0;
    	while (std::getline(ScalaFile, tempString)) {
    	    char firstChar = tempString.front();
    	    if (firstChar == '!'){
    	        continue;
    	    }
    	    tempString = reduce(tempString);
    	    
    	    if (tempString.find(".") != string::npos){
    	        i[NoteIntervals] = centsToRatio(std::stod(tempString));
    	    }else{
    	        std::stringstream ss(tempString);
    	        string t;
    	        string newstring[2];
    	        std::getline(ss, t, '/');
    	        newstring[0] = t;
    	        std::getline(ss, t, '/');
    	        newstring[1] = t;
    	        i[NoteIntervals] = std::stod(newstring[0])/std::stod(newstring[1]);
    	    }
    	    
    	    i++;
    	    if (i == NumNotes-1){
    	        break;
    	    }
    	}
    	ScalaFile.close();
    	ScaleName = PeiceTitle;
	
	    
	    return NoteIntervals;
}
	
int main(int argc, char **argv){
	    
	string ScaleName;
	int NumNotes;
	double* NoteIntervals = GetScaleData(ScaleName, NumNotes);
	    
	std::cout << "Enter a starting frequency"<<std::endl;
	string response;
	std::cin >> response;
	freq startFrequency = std::stod(reduce(response));
	double NoteInterval[NumNotes];
	std::copy(std::begin(NoteIntervals), std::end(NoteIntervals),std::begin(NoteInterval));	
	NoteIntervalArray nia = {0};
	nia.NoteInterval = NoteInterval;
	nia.size = NumNotes;
	

    	free(NoteIntervals);
	
    	GtkApplication *app;
	int status;
	
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (activate), (gpointer)nia);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);

	return status;    
}
