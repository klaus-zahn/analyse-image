#include <opencv2/opencv.hpp>

cv::Mat img;
bool selectObject = false;
cv::Point origin;
cv::Rect selection;
int analyseRect = 0;
int threshold = 10;


static void onMouse( int event, int x, int y, int, void* );


int main(int argc, char *argv[])
{
    std::string fileName;

    //check for additional argument
    if (argc == 1) {
        //if not given try to read the default image
        fileName = "Spielsteine.png";
    }
    else if (argc == 2) {
        //read given image
        fileName = argv[1];
    }

    //cv::Mat is the class for handling images in opencv 
    cv::Mat imgOrig = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
    if (imgOrig.rows*imgOrig.cols <= 0)
    {
        std::cout << "Image " << fileName << " is empty or cannot be found\n";
        return(0);
    }       
    
    cv::namedWindow( "original image" );
    cv::setMouseCallback( "original image", onMouse, 0 );
    cv::createTrackbar( "threshold", "original image", &threshold, 256, 0 );    
    
    
    cv::Scalar avg(0);
    cv::Mat mask;
    bool go = true;
    while(go) {
        
        cv::Mat imgShow = imgOrig.clone(); 
        img = imgOrig;
        
        if(analyseRect == 1) {
            cv::Mat roi(img, selection);
            
            avg = cv::mean(roi);
            
            int i0 = 0;
            while(i0 < 4 && avg[i0] != 0) {
                std::cout << "channel " << i0 << "; avg = " << avg[i0] << std::endl;
                i0++;
            }
            
            analyseRect = 2;
        }
        
        if(analyseRect == 2) {
            cv::inRange(img, cv::Scalar(avg[0]-threshold),
                             cv::Scalar(avg[0]+threshold), mask);
            cv::imshow("mask image", mask);
        }
        
        if( selectObject && selection.width > 0 && selection.height > 0 ) {
            cv::Mat roi(imgShow, selection);
            cv::bitwise_not(roi, roi);
        }
        
         cv::imshow("original image", imgShow);
        
        char c = (char) cv::waitKey(1);
        switch(c) {
            case 'q':
            case 'Q':
                go = false;
                break;
        }        
    }
    
    return(0);
}


static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);

        selection &= cv::Rect(0, 0, img.cols, img.rows);
    }

    switch( event )
    {
        case cv::EVENT_LBUTTONDOWN:
            origin = cv::Point(x,y);
            selection = cv::Rect(x,y,0,0);
            selectObject = true;
            break;
        case cv::EVENT_LBUTTONUP:
            selectObject = false;
            if( selection.width > 0 && selection.height > 0 )
                analyseRect = 1;
            break;
    }
}