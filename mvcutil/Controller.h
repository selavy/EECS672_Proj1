// Controller.h - a basic Controller (in Model-View-Controller sense)

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <string>

class ModelView;

class Controller
{
public:
	Controller(const std::string& name);
	virtual ~Controller();

	void addModel(ModelView* m);
	double getViewportAspectRatio() const; // height/width
	void getWCRegionOfInterest(double* xyzLimits) const;

	static bool checkForErrors(std::ostream& os, const std::string& context);
	static Controller* getCurrentController() { return curController; }
	static void reportVersions(std::ostream& os);

protected:
	Controller(const Controller& c) {} // do not allow copies, including pass-by-value
	ModelView* model;

	virtual void establishInitialCallbacksForRC();
	virtual void handleDisplay();
	virtual void handleKeyboard(unsigned char key, int x, int y);
	virtual void handleReshape();

	void screenXYToLDS(int x, int y, double& ldsX, double& ldsY);

	static int createWindow(const std::string& windowTitle);

	// Data the Controller uses to track the overall WC box bounding all models.
	double overallWCBoundingBox[6];

private:
	// Width and Height of current viewport (window manager window)
	int vpWidth, vpHeight;

	static Controller* curController;

	static void displayCB();
	static void keyboardCB(unsigned char key, int x, int y);
	static void reshapeCB(int width, int height);
};

#endif
