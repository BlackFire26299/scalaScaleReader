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
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Sleep.hpp>
#include <vector>


typedef std::string string;
typedef double cents;
typedef double freq;
typedef double interval;
typedef struct {
	double* NoteInterval;
	int size;
	double frequency;

} NoteIntervalArray;

string Filename;

static void start_scale (GtkWidget *widget, gpointer data)
{
	g_print("Starting scale... \n");
	NoteIntervalArray* nia = (NoteIntervalArray*)data;
	std::vector<double> frequencies;
	frequencies.push_back(nia->frequency);
	double newfreq = nia->frequency;
	std::cout << 1 << ". : "<<newfreq << " : " << newfreq << std::endl;
	for(int i = 0; i<nia->size; i++){
		
		newfreq = nia->frequency * nia->NoteInterval[i];
		frequencies.push_back(newfreq);
		std::cout << i+1 << ". : "<<nia->NoteInterval[i] << " : " << newfreq << std::endl;
	}
	
	
	



	std::size_t samplesPerCycle = 44100;
	std::size_t SAMPLES = samplesPerCycle*frequencies.size();
	constexpr std::size_t SAMPLE_RATE = 44100;

	std::vector<std::int16_t> raw(SAMPLES);

	constexpr std::int16_t AMPLITUDE = 5000;
	constexpr double TWO_PI = 6.28318;
	double increment = frequencies.front() / 44100.0;
	double x = 0.0;
	std::cout << SAMPLES << std::endl;
	for (std::size_t j = 0; j<frequencies.size(); j++){
		increment = frequencies.at(j) / 44100.0;
		for (std::size_t i = 0; i < samplesPerCycle; ++i)
		{
			
			raw.at((size_t)(j*44100+i)) = static_cast<std::int16_t>(AMPLITUDE * sin(x * TWO_PI));
			x += increment;
		}
		x = 0.0;
	}
	sf::SoundBuffer buffer{};
	if (!buffer.loadFromSamples(raw.data(), raw.size(), 1, SAMPLE_RATE, { sf::SoundChannel::Mono }))
	{
		std::cerr << "Loading failed!" << std::endl;
		
	}

	sf::Sound sound(buffer);
	sound.play();
	while (sound.getStatus() == sf::SoundSource::Status::Playing)
	{
		sf::sleep(sf::milliseconds(100));
	}
	
	
    
}

static void on_open_response(GtkDialog *dialog, int response) {
    if (response == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        GFile *file = gtk_file_chooser_get_file(chooser);
        Filename = g_file_get_parse_name(file);
		std::cout << Filename << std::endl;
		
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
}


static void choose_file(GtkWidget *button, gpointer data){

	GtkWidget *dialog;
GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

dialog = gtk_file_chooser_dialog_new("Open File", (GtkWindow*)data, action,
                              "_Cancel", GTK_RESPONSE_CANCEL,
                              "_Open", GTK_RESPONSE_ACCEPT, NULL);

gtk_window_present(GTK_WINDOW(dialog));

g_signal_connect(dialog, "response", G_CALLBACK(on_open_response), NULL);

	

    

}
static void
activate (GtkApplication *app, gpointer user_data)
{
  	GtkWidget *window;
  	GtkWidget *button;
	GtkWidget *chooseFileButton;
	GtkWidget *grid = gtk_grid_new();

	
	
  	window = gtk_application_window_new (app);
  	gtk_window_set_title (GTK_WINDOW (window), "Scala Scale reader");
  	gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);
	
	button = gtk_button_new_with_label ("Start Scale");
  	gtk_widget_set_halign(button, GTK_ALIGN_CENTER);
  	gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
	gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1,1);
	//gtk_window_set_child (GTK_WINDOW (window), button);
  	g_signal_connect (button, "clicked", G_CALLBACK (start_scale), user_data);

	
  	gtk_window_set_child (GTK_WINDOW (window), button);
	chooseFileButton = gtk_button_new_with_label ("Choose file");
  	gtk_widget_set_halign(chooseFileButton, GTK_ALIGN_CENTER);
  	gtk_widget_set_valign(chooseFileButton, GTK_ALIGN_CENTER);
	gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1,1);
	//gtk_window_set_child (GTK_WINDOW (window), chooseFileButton);
	g_signal_connect (chooseFileButton, "clicked", G_CALLBACK (choose_file), (gpointer)window);
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
	gtk_window_set_child(GTK_WINDOW(window), grid);

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
    	    std::cout << tempString << std::endl;
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
    	    if (i == NumNotes){
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
	NoteIntervalArray nia = {0};
	nia.NoteInterval = NoteIntervals;
	nia.size = NumNotes;
	nia.frequency = startFrequency;
	NoteIntervalArray* pnia = &nia;	

    
	
    GtkApplication *app;
	int status;
	
	app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK (activate), (gpointer)pnia);
	status = g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	free(NoteIntervals);
	return status;    
}
