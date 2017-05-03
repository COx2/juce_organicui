/*
 ==============================================================================

 Logger.h
 Created: 6 May 2016 1:37:41pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef CUSTOMLOGGER_H_INCLUDED
#define CUSTOMLOGGER_H_INCLUDED

class CustomLogger :
	public Logger
{
public :
    juce_DeclareSingleton(CustomLogger, true);

	CustomLogger();
	
	~CustomLogger() { if (Logger::getCurrentLogger() == this) Logger::setCurrentLogger(nullptr); }

	void logMessage(const String & message) override;


    QueuedNotifier<String> notifier;
    typedef QueuedNotifier<String>::Listener Listener;



    void addLogListener(Listener * l){notifier.addListener(l);}
    void removeLogListener(Listener * l){notifier.removeListener(l);}

    class FileWriter : public Listener{
    public:
        FileWriter(){fileLog = FileLogger::createDefaultAppLogger("Chataigne", "log", "");}

        void newMessage(const String& s) override{if (fileLog) {fileLog->logMessage(s);}}
        String getFilePath(){return fileLog->getLogFile().getFullPathName();}
        ScopedPointer<FileLogger> fileLog;
    };

    FileWriter fileWriter;


	//SCRIPT
	static var logFromScript(const var::NativeFunctionArgs &args);
};



#endif  // CUSTOMLOGGER_H_INCLUDED
