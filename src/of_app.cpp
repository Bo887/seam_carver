#include "of_app.h"
#include "popup.h"
#include "image_utils.h"
#include "constants.h"

OfApp::OfApp(shared_ptr<ofAppBaseWindow>& main_window){
    this->main_window = main_window;
    gif_generated = false;
}

void OfApp::setup(){
    load.addListener(this, &OfApp::loadImage);
    popup_carved.addListener(this, &OfApp::popupCarved);
    show_gif.addListener(this, &OfApp::popup_gif);
    start_calculation.addListener(this, &OfApp::startCalculation);

    panel.setup();
    panel.add(load.setup("Load Image"));
    panel.add(enable_face_detection.setup("Enable Face Detection?", true));
    panel.add(enable_gif_generation.setup("Enable Gif Calculation?", true));
    panel.add(popup_carved.setup("Display Processed Image (Popup)"));
    panel.add(show_gif.setup("Display GIF of Processing (Popup)"));
    panel.add(target_height.setup("Set Target Height", image.getHeight())); 
    panel.add(target_width.setup("Set Target Width", image.getWidth()));
    panel.add(image_height.setup("Image Height", ""));
    panel.add(image_width.setup("Image Width", ""));
    panel.add(start_calculation.setup("Calculate!"));
    panel.setSize(constants::kPanelWidth, constants::kPanelHeight);
    panel.setWidthElements(constants::kPanelWidth);

    progress = 0;
    progress_bar = ofxProgressBar(constants::kProgressBarX, constants::kProgressBarY, constants::kProgressBarWidth, constants::kProgressBarHeight, &progress, 100);
    ofTrueTypeFont font;
    font.load("verdana.ttf", constants::kProgressBarFontSize, true, true);
    progress_bar.setFont(font);
    progress_bar.setBarColor(ofColor::white);
    progress_bar.setBackgroundColor(ofColor::black);
    progress_bar.setLabel("Seam Carving Progress...");

    face_detector.setup("haarcascade_frontalface_default.xml");
}

void OfApp::loadImage(){
    ofFileDialogResult result = ofSystemLoadDialog("Load file");
    if (result.bSuccess){
        string path = result.getPath();
        image.load(path);
        const int max_size = constants::kMaxLoadedImageDimension;
        int bigger = max(image.getHeight(), image.getWidth());
        image.resize(max_size*image.getWidth()/bigger, max_size*image.getHeight()/bigger);
        image_height = ofToString(image.getHeight());
        image_width = ofToString(image.getWidth());
    }
    if (result.bSuccess){
        face_detector.update(image);
    }

    //reset the processed images to the new image
    carved_image = image;
}

void OfApp::popupCarved(){
    if (!image.isAllocated()){
        return;
    }
    runImagePopupWindow(carved_image, main_window);
}

void OfApp::popup_gif(){
    if (!image.isAllocated() || !gif_generated){
        return;
    }
    int h = max(image.getHeight(), carved_image.getHeight());
    int w = max(image.getWidth(), carved_image.getWidth());
    runGifPopupWindow("test.gif", h, w, main_window);
}

void OfApp::startCalculation(){
    if (!image.isAllocated()){
        return;
    }
    if (background_runner.started() && !background_runner.finished()){
        return;
    }
    if (target_height <= 0 || target_width <= 0){
        cout << "INVALID INPUT" << endl;
        return;
    }
    int diff_height = image.getHeight() - target_height;
    int diff_width = image.getWidth() - target_width;
    string path = ofToDataPath("test.gif");
    gif_generated = enable_gif_generation;
    FaceBounds face_bounds = constants::kNoFaceBounds;
    if (enable_face_detection){
        ofRectangle face = face_detector.getObject(0);
        face_bounds.x = face.getMinX();
        face_bounds.y = face.getMinY();
        face_bounds.upper_x = face.getMaxX();
        face_bounds.upper_y = face.getMaxY();
        cout << face_bounds << endl;
    }
    background_runner.start(image, face_bounds, path, diff_height, diff_width, enable_gif_generation);
}

void OfApp::update(){
    if (background_runner.started() && !background_runner.finished()){
        progress = background_runner.get_progress();
    }
    if (background_runner.started() && background_runner.finished()){
        progress = 0;
        background_runner.stop();
        carved_image = image_utils::raw_to_of(background_runner.getProcessedImage());
    }
}

void OfApp::draw(){
    ofBackgroundGradient(ofColor::aliceBlue, ofColor::lightBlue);
    panel.draw();
    progress_bar.draw();
    if (image.isAllocated()){
        image.draw(constants::kLoadedImageX, constants::kLoadedImageY);
        if (enable_face_detection){
            ofNoFill();
            ofRectangle object = face_detector.getObject(0);
            object.setX(constants::kLoadedImageX + object.getX());
            object.setY(constants::kLoadedImageY + object.getY());
            ofDrawRectangle(object);
        }
    }
}

void OfApp::exit(){
    background_runner.stop();
}
