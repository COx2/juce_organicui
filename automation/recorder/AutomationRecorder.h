/*
  ==============================================================================

	AutomationRecorder.h
	Created: 12 Jan 2018 9:55:22pm
	Author:  Ben

  ==============================================================================
*/

#pragma once

class AutomationRecorder :
	public EnablingControllableContainer,
	public Inspectable::InspectableListener
{
public:
	AutomationRecorder();
	~AutomationRecorder();

	TargetParameter * input;
	FloatParameter * simplificationFactor;
	BoolParameter * arm;
	BoolParameter * autoDisarm;

	WeakReference<Parameter> currentInput;

	BoolParameter * isRecording;

	Array<Point<float>> keys;

	void setCurrentInput(Parameter * input);

	void clearKeys();
	void addKeyAt(float time);

	void startRecording();
	void cancelRecording();
	Array<Point<float>> stopRecordingAndGetKeys();

	bool shouldRecord();

	void onContainerParameterChanged(Parameter * p) override;
	
	void inspectableDestroyed(Inspectable * i) override;

	InspectableEditor * getEditor(bool isRoot) override;

	static Array<Point<float>> getSimplifiedKeys(Array<Point<float>> arr, float epsilon);
	static const std::pair<int, float> findMaximumDistance(Array<Point<float>> arr);
};