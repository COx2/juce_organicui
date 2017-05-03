/*
  ==============================================================================

    Point2DParameter.h
    Created: 30 Sep 2016 9:37:36am
    Author:  bkupe

  ==============================================================================
*/

#ifndef POINT2DPARAMETER_H_INCLUDED
#define POINT2DPARAMETER_H_INCLUDED

class Point2DParameter : public Parameter
{
public:
	Point2DParameter(const String &niceName, const String &description, bool enabled = true);
	~Point2DParameter() {}

	float x, y;
	void setPoint(Point<float> value);
	void setPoint(float x, float y);
	void setValueInternal(var & _value) override;
	
	void setBounds(float _minX, float _minY, float _maxX, float _maxY);
	
	Point<float> getPoint();
	
	bool checkValueIsTheSame(var newValue, var oldValue) override;

	ControllableUI * createDefaultUI(Controllable * targetControllable = nullptr) override;

	static Point2DParameter * create() { return new Point2DParameter("New Point2D Parameter", "", 0); }
	virtual String getTypeString() const override { return "Point 2D"; }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Point2DParameter)
};


#endif  // POINT2DPARAMETER_H_INCLUDED
