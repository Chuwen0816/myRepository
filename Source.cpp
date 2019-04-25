#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <string>

using namespace cv;
using namespace std;

Mat detectSkinColor(Mat img, int height, int width);
Mat detectEdges(Mat img);
Mat seperateFingers(Mat img_edge, Mat img_skin, int height, int width);
Mat selectFingers(Mat img, int height, int width, int area);

class StringsNFrets
{
public:
	void printSth() const
	{
		cout << "Just printing something.\n";
	} 
	void setPoint1(double x, double y)
	{
		Point1.x = x;
		Point1.y = y;
	}
	void setPoint2(double x, double y)
	{
		Point2.x = x;
		Point2.y = y;
	}
	void setPoint3(double x, double y)
	{
		Point3.x = x;
		Point3.y = y;
	}
	Point2d getPoint1() const
	{
		return Point1;
	}
	Point2d getPoint2() const
	{
		return Point2;
	}
	Point2d getPoint3() const
	{
		return Point3;
	}
	double getDistance1() const
	{
		return sqrt(pow(Point2.x - Point1.x, 2) + pow(Point2.y - Point1.y, 2));
	}
	double getDistance2() const
	{
		return sqrt(pow(Point3.x - Point1.x, 2) + pow(Point3.y - Point1.y, 2));
	}
private:
	Point2d Point1;
	Point2d Point2;
	Point2d Point3;
	Point2d Point4;
};

int main()
{
	Mat img_empty = imread("empty.jpg");
	Mat img_C = imread("C.jpg");
	Mat img_Am = imread("Am.jpg");
	Mat img_Fmaj7 = imread("Fmaj7.jpg");

	int height = 720;
	int width = 1280;
	Mat img_Now = img_Fmaj7;

	Mat img_grayscale(height, width, CV_8UC1, Scalar(0));
	cvtColor(img_Now, img_grayscale, COLOR_BGR2GRAY);

	//imshow("Original image", img_Now);
	//waitKey(0);

	//skin color detection
	//detectSkinColor: Check all pixel's BGR values of input image, mark the skin.
	//Return a grayscale image with skin color marked.

	//Mat img_skin = detectSkinColor(img_Now, height, width);
	//Mat SkinArea;
	//dilate(img_skin, SkinArea, Mat());                                                                                                                                                                                          
	//imshow("Dilated Skin Area Detection", SkinArea);
	//waitKey(0);

	//filter2D
	Mat img_laplace;
	Mat img_blurred;
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, 0, 5, 0, 0, -1, 0);

	GaussianBlur(img_grayscale, img_blurred, Size(3, 3), 0, 0);
	filter2D(img_blurred, img_laplace, CV_8UC1, kernel);
	imshow("Laplace enhencement", img_laplace);
	waitKey(0);

	//canny edge detection
	//detectEdges: Convert input BGR image into grayscale, perform Gaussian blur and Canny edge detection.
	//Return a grayscale image with edges marked.
	//Mat img_edge = detectEdges(img_laplace);
	//imshow("Edge Detection1", img_edge);
	//waitKey(0);

	//Threshold
	Mat img_binary(height, width, CV_8UC1, Scalar(0));
	threshold(img_laplace, img_binary, 80, 255, THRESH_BINARY);
	imshow("Threshold after Laplace enhencement", img_binary);
	waitKey(0);

	Mat img_edge2 = detectEdges(img_binary);
	//imshow(" Edge Detection2", img_edge2);
	//waitKey(0);

	//finger detection
	//seperateFingers: If the marked pixels(white) in Edge and SkinArea are at the same position, make it unmarked(black).
	//Return a grayscale image with fingers seperated.

	//Mat img_finger = seperateFingers(Edge, SkinArea, height, width);
	//imshow("Finger Detection", img_finger);
	//waitKey(0);

	//finger selection

	//Mat img_selected = selectFingers(img_finger, height, width);
	//imshow("Selected Fingers", img_selected);
	//waitKey(0);

	//feature
	/*
	Mat harrisStrength;
	cornerHarris(img_binary, harrisStrength, 3, 3, 0.01);

	Mat corners(height, width, CV_8UC1, Scalar(0));
	float threshold = 0.03;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			float strength;
			strength = harrisStrength.at<float>(i, j);
			if ((strength > threshold))
			{
				corners.at<uchar>(i, j) = 255;
			}
		}
	}

	imshow("corners", corners);
	waitKey(0);
	*/

	//Hough lines
	vector<Vec4i> linesH;
	vector<Vec4i> linesV;

	Mat img_Strings(height, width, CV_8UC1, Scalar(0));
	Mat img_Frets(height, width, CV_8UC1, Scalar(0));

	imshow("edge2", img_edge2);
	waitKey(0);

	Mat img_Edge;
	Mat ellipseKernal3, ellipseKernal5, rectKernal3, rectKernal5, crossKernal3, crossKernal5;
	ellipseKernal3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	ellipseKernal5 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	rectKernal3 = getStructuringElement(MORPH_RECT, Size(3, 3));
	rectKernal5 = getStructuringElement(MORPH_RECT, Size(5, 5));
	crossKernal3 = getStructuringElement(MORPH_CROSS, Size(3, 3));
	crossKernal5 = getStructuringElement(MORPH_CROSS, Size(5, 5));

	dilate(img_edge2, img_Edge, rectKernal5, Point(-1, -1), 3);
	imshow("dilated", img_Edge);
	waitKey(0);
	
	Mat Mblur;
	medianBlur(img_Edge, Mblur, 5);
	imshow("medianBlur", Mblur);
	waitKey(0);

	Mat houghInputH, houghInputV;
	erode(Mblur, houghInputH, rectKernal3, Point(-1, -1), 6);
	erode(houghInputH, houghInputV, rectKernal3, Point(-1, -1), 1);
	imshow("erode1", houghInputH);
	imshow("erode2", houghInputV);
	waitKey(0);

	HoughLinesP(houghInputH, linesH, 1, CV_PI / 180, 100, 200, 15);
	HoughLinesP(houghInputV, linesV, 2, CV_PI / 90, 75, 100, 15);

	for (int i = 0; i < linesH.size(); i++)
	{
		if (linesH[i][0] == 0)
		{
			line(img_Strings, Point(linesH[i][0], linesH[i][1]), Point(linesH[i][2], linesH[i][3]), Scalar(255));
		}
	}
	imshow("Horizontal Lines", img_Strings);
	waitKey(0);

	for (int i = 0; i < linesV.size(); i++)
	{
		if ((abs(linesV[i][3] - linesV[i][1]) / (abs(linesV[i][2] - linesV[i][0]) + 1)) > 3 && (linesV[i][2] <= 320) && (linesV[i][0] <= 320))
		{
			line(img_Frets, Point(linesV[i][0], linesV[i][1]), Point(linesV[i][2], linesV[i][3]), Scalar(255));
		}
	}
	imshow("Vertical Lines", img_Frets);
	waitKey(0);

	Mat img_crossPixels(height, width, CV_8UC1, Scalar(0));
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img_Strings.at<uchar>(i, j) && img_Frets.at<uchar>(i, j))
			{
				img_crossPixels.at<uchar>(i, j) = 255;
			}
		}
	}
	imshow("Cross points", img_crossPixels);
	waitKey(0);

	Mat img_gatheringPoints;
	dilate(img_crossPixels, img_gatheringPoints, ellipseKernal5, Point(-1, -1), 3);
	imshow("Points", img_gatheringPoints);
	waitKey(0);

	Mat img_rotation(height, width, CV_8UC1, Scalar(0));
	Mat img_coloredRotation(height, width, CV_8UC3, Scalar(0));
	Point center = Point(width / 2, height / 2);
	double angle = -15.0;
	double scale = 1;
	Mat rot_mat = getRotationMatrix2D(center, angle, scale);
	warpAffine(img_gatheringPoints, img_rotation, rot_mat, img_rotation.size());
	warpAffine(img_Now, img_coloredRotation, rot_mat, img_coloredRotation.size());
	imshow("After roatation", img_rotation);
	waitKey(0);

	Mat labels, stats, centroids;
	Mat img_selected(height, width, CV_8UC1, Scalar(0));
	int nccomps = connectedComponentsWithStats(img_rotation, labels, stats, centroids); //variable enter here

	for (int i = 1; i < nccomps; i++)
	{
		//int left, top;
		//left = stats.at<int>(i, CC_STAT_LEFT);
		//top = stats.at<int>(i, CC_STAT_TOP);
		//img_selected.at<Vec3b>(top, left) = Vec3b(255, 255, 255);


		double Xc, Yc;
		Xc = centroids.at<double>(i, 0);
		Yc = centroids.at<double>(i, 1);
		//cout << Xc << " " << Yc << endl;
		img_selected.at<uchar>(Yc, Xc) = 255;
	}

	imshow("Selected points", img_selected);

	StringsNFrets SnF;
	if (centroids.at<double>(1, 0) > centroids.at<double>(2, 0)) //向右上偏
	{
		for (int i = 1; i < nccomps; i++)
		{
			if (centroids.at<double>(i + 1, 0) > centroids.at<double>(i, 0))
			{
				SnF.setPoint1(centroids.at<double>(i, 0), centroids.at<double>(i, 1));
				SnF.setPoint2(centroids.at<double>(i - 1, 0), centroids.at<double>(i - 1, 1));
				SnF.setPoint3(centroids.at<double>(nccomps - 1, 0) , centroids.at<double>(nccomps - 1, 1));
				break;
			}
		}
		cout << endl << SnF.getDistance1() << endl << SnF.getDistance2() << endl;
	}
	else //向右下偏
	{
		for (int i = nccomps - 1; i > 0; i--)
		{
			if (centroids.at<double>(i - 1, 0) > centroids.at<double>(i, 0))
			{
				SnF.setPoint1(centroids.at<double>(1, 0), centroids.at<double>(1, 1));
				SnF.setPoint2(centroids.at<double>(2, 0), centroids.at<double>(2, 1));
				SnF.setPoint3(centroids.at<double>(i, 0), centroids.at<double>(i, 1));
				break;
			}
		}
		cout << endl << SnF.getDistance1() << endl << SnF.getDistance2() << endl;
	}

	Point2f srcTri[3];
	srcTri[0] = SnF.getPoint1();
	srcTri[1] = SnF.getPoint2();
	srcTri[2] = SnF.getPoint3();

	Point2f dstTri[3];
	dstTri[0] = SnF.getPoint1();
	dstTri[1] = Point2f(SnF.getPoint1().x + SnF.getDistance1(), SnF.getPoint1().y);
	dstTri[2] = Point2f(SnF.getPoint1().x, SnF.getPoint1().y + SnF.getDistance2());

	Mat warp_mat = getAffineTransform(srcTri, dstTri);
	Mat img_dst(height, width, CV_8UC1, Scalar(0));
	Mat img_dst2(height, width, CV_8UC3, Scalar(0));
	warpAffine(img_selected, img_dst, warp_mat, img_dst.size());
	warpAffine(img_coloredRotation, img_dst2, warp_mat, img_dst2.size());
	
	imshow("Adjusted", img_dst);
	imshow("img_Now", img_dst2);
	waitKey(0);



	int up = 720;
	int down = 0;
	int left = 1280;
	int right = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img_crossPixels.at<uchar>(i, j))
			{
				if (i > down)
					down = i;
				if (i < up)
					up = i;
				if (j > right)
					right = j;
				if (j < left)
					left = j;
			}
		}
	}

	if (left - 50 >= 0)
		left -= 50;
	else
		left = 0;

	if (right + 200 <= 1280)
		right += 200;
	else
		right = 1280;

	if (up - 50 >= 0)
		up -= 50;
	else
		up = 0;

	if (down + 50 <= 720)
		down += 50;
	else
		down = 720;

	Mat ROI = img_Now(Rect(left, up, right - left, down-up));
	//imshow("ROI", ROI);
	//waitKey(0);	
	
	return 0;
}

Mat detectSkinColor(Mat img, int height, int width)
{
	Mat img_skin(height, width, CV_8UC1, Scalar(0));

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vec3b pixel;
			pixel = img.at<Vec3b>(i, j);
			if ((pixel[0] > 20) && (pixel[1] > 40) && (pixel[2] > 95) && (pixel[2] - pixel[0] > 20) && (pixel[2] - pixel[1] > 10))
			{
				img_skin.at<uchar>(i, j) = 255;
			}
		}
	}
	return img_skin;
}
Mat detectEdges(Mat img_grayscale)
{
	Mat  img_blurred, img_edge;
	
	GaussianBlur(img_grayscale, img_blurred, Size(5, 5), 0, 0);
	Canny(img_blurred, img_edge, 30, 90, 3);
	return img_edge;
}
Mat seperateFingers(Mat img_edge, Mat img_skin, int height, int width)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (img_edge.at<uchar>(i, j))
			{
				img_skin.at<uchar>(i, j) = 0;
			}
		}
	}
	return img_skin;
}
Mat selectFingers(Mat img, int height, int width, int area)
{
	Mat labels, stats, centroids;
	Mat img_selected(height, width, CV_8UC3, Vec3b(0, 0, 0));
	int nccomps = connectedComponentsWithStats(img, labels, stats, centroids);

	vector<Vec3b> colors(nccomps);
	colors[0] = Vec3b(0, 0, 0); // background pixels remain black. 
	for (int i = 1; i < nccomps; i++)
	{
		colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
		if (stats.at<int>(i, CC_STAT_AREA) < area)
			colors[i] = (0, 0, 0); // small regions are painted with black too.
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int label = labels.at<int>(i, j);
			CV_Assert(0 <= label && label <= nccomps); //assure if the value of label is valid.
			img_selected.at<Vec3b>(i, j) = colors[label];
		}
	}

	for (int i = 1; i < nccomps; i++)
	{
		//int left, top;
		//left = stats.at<int>(i, CC_STAT_LEFT);
		//top = stats.at<int>(i, CC_STAT_TOP);
		//img_selected.at<Vec3b>(top, left) = Vec3b(255, 255, 255);

		double Xc, Yc;
		Xc = centroids.at<double>(i, 0);
		Yc = centroids.at<double>(i, 1);
		cout << Xc << " " << Yc << endl;
		img_selected.at<Vec3b>(Yc, Xc) = Vec3b(255, 255, 255);
	}

	double distance1 = 0.0, distance2 = 0.0;
	if (centroids.at<double>(1, 0) > centroids.at<double>(2, 0)) //向右上偏
	{
		for (int i = 1; i < nccomps; i++)
		{
			if (centroids.at<double>(i + 1, 0) > centroids.at<double>(i, 0))
			{
				distance1 = sqrt(pow(centroids.at<double>(i - 1, 0) - centroids.at<double>(i, 0), 2) + pow(centroids.at<double>(i - 1, 1) - centroids.at<double>(i, 1), 2));
				distance2 = sqrt(pow(centroids.at<double>(nccomps - 1, 0) - centroids.at<double>(i, 0), 2) + pow(centroids.at<double>(nccomps - 1, 1) - centroids.at<double>(i, 1), 2));
				break;
			}
		}
		cout << endl << distance1 << endl << distance2 << endl;
	}
	else //向右下偏
	{
		distance1 = sqrt(pow(centroids.at<double>(1, 0) - centroids.at<double>(2, 0), 2) + pow(centroids.at<double>(1, 1) - centroids.at<double>(2, 1), 2));
		for (int i = nccomps - 1; i > 0; i--)
		{
			if (centroids.at<double>(i - 1, 0) > centroids.at<double>(i, 0))
			{
				distance2 = sqrt(pow(centroids.at<double>(1, 0) - centroids.at<double>(i, 0), 2) + pow(centroids.at<double>(1, 1) - centroids.at<double>(i, 1), 2));
				break;
			}
		}
		cout << endl << distance1 << endl << distance2 << endl;
	}
	return img_selected;
}