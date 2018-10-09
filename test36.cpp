
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
#include <io.h>
#include <Windows.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <direct.h>
using namespace cv;
using namespace std;

int test36_1(int argc,char * argv[])
{
	Mat img = imread("20180831103839.jpg");
	Mat bw;
	bool dRect;
	pyrDown(img,img);
	pyrDown(img, img);
	cvtColor(img,bw,COLOR_BGR2GRAY);
	threshold(bw,bw,150,255,CV_THRESH_BINARY);
	vector <vector<Point>> contours;
	vector<Vec4i>hierarchy;
	findContours(bw,contours,hierarchy,CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size();i++)
	{
		RotatedRect minRect = minAreaRect(Mat(contours[i]));
		Point2f rect_points[4];
		minRect.points(rect_points);
		for (int j = 0; j < 4; j++)
		{
			line(img,rect_points[j],rect_points[(j+1)%4],Scalar(255,255,0),2);
		}
	}

	imshow("img",img);
	waitKey();
	return 0;
}

Point2f GetPointAfterRotate(Point2f inputpoint, Point2f center, double angle)
{
	Point2d pretu;
	pretu.x = (inputpoint.x - center.x)*cos(angle) - (inputpoint.y - center.y)*sin(angle);
	pretu.y = (inputpoint.x - center.x)*sin(angle) + (inputpoint.y - center.y)*cos(angle);

	return pretu;
}


Point GetPointAfterRotate(Point inputpoint, Point center, double angle)
{
	Point pretu;
	pretu.x = (inputpoint.x - center.x)*cos(angle) - (inputpoint.y - center.y)*sin(angle);
	pretu.y = (inputpoint.x - center.x)*sin(angle) + (inputpoint.y - center.y)*cos(angle);

	return pretu;
}


double getOrientation(vector<Point2f> & pts, Point2f & pos, Mat & img)
{
	Mat data_pts = Mat(pts.size(),2,CV_64FC1);
	for (int i = 0; i < data_pts.rows;i++)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}

	PCA pca_analysis(data_pts,Mat(),CV_PCA_DATA_AS_ROW);
	pos = Point2f(pca_analysis.mean.at<double>(0, 0), pca_analysis.mean.at<double>(0, 1));
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2;++i)
	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0), pca_analysis.eigenvectors.at<double>(i, 1));
	    eigen_val[i] = pca_analysis.eigenvalues.at<double>(i, 0);
	}

	return atan2(eigen_vecs[0].y, eigen_vecs[0].x);
}

int test36_2(int argc, char * argv[])
{
	Mat img = imread("20180831103839.jpg");
	Mat bw;
	bool dRect;
	pyrDown(img, img);
	pyrDown(img, img);
	cvtColor(img, bw, COLOR_BGR2GRAY);
	threshold(bw, bw, 150, 255, CV_THRESH_BINARY);
	vector <vector<Point2f>> contours;
	vector<Vec4i>hierarchy;
	findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area<1e2||area>1e5)
		{
			continue;
		}
		drawContours(img, contours, i, CV_RGB(255, 0, 0), 2, 8, hierarchy, 0);
		//Point2f* pos = new Point2f();
		Point2f  pos;
		double dOrient = getOrientation(contours[i], pos, img);
		for (size_t j = 0; j < contours[i].size();j++)
		{
			contours[i][j] = GetPointAfterRotate(contours[i][j], pos, dOrient);
		}
		Rect rect = boundingRect(contours[i]);

		//RotatedRect rotateRect = RotatedRect((Point2f)rect.tl(), (Size2f)(rect.br().x, rect.br().y), (Point2f)rect.br());
		RotatedRect rotateRect;
	

		Point2f rect_points[4];
		rotateRect.points(rect_points);
		for (size_t j = 0; j < 4; j++)
		{
			rect_points[j] = GetPointAfterRotate((Point2f)rect_points[j], pos, dOrient);
		}
		for (int j = 0; j < 4; j++)
		{
			line(img, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 255, 0), 2);
		}
		char cbuf[255];
		double fshort = min(rect.width, rect.height);
		double flong = max(rect.width, rect.height);
		sprintf(cbuf, "第%d个轮廓,长度%0.2f,宽度%0.2f像素\n", i, flong, fshort);
	}

	
	return 0;
}


int test36_3(int argc, char * argv[])
{
	Mat image(500, 500, CV_8UC3, Scalar(0));
	RotatedRect rRect(Point2f(250, 250), Size2f(100, 50), -30);
	Point2f vertices[4];      //定义矩形的4个顶点  
	rRect.points(vertices);   //计算矩形的4个顶点  
	for (int i = 0; i < 4; i++)
		line(image, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
	Rect brect = rRect.boundingRect(); //返回包含旋转矩形的最小矩形  
	rectangle(image, brect, Scalar(255, 0, 0));

	float a1 = sqrt((vertices[0].x - vertices[1].x)*(vertices[0].x - vertices[1].x) + (vertices[0].y - vertices[1].y)*(vertices[0].y - vertices[1].y));
	float a2 = sqrt((vertices[1].x - vertices[2].x)*(vertices[1].x - vertices[2].x) + (vertices[1].y - vertices[2].y)*(vertices[1].y - vertices[2].y));
	float a3 = sqrt((vertices[2].x - vertices[3].x)*(vertices[2].x - vertices[3].x) + (vertices[2].y - vertices[3].y)*(vertices[2].y - vertices[3].y));
	float a4 = sqrt((vertices[3].x - vertices[0].x)*(vertices[3].x - vertices[0].x) + (vertices[3].y - vertices[0].y)*(vertices[3].y - vertices[0].y));

	imwrite("rectangles.jpg", image);
	imshow("rectangles", image);
	waitKey(0);
	return 0;
}

int test36_4(int argc, char * argv[])
{
	

	/*Mat img = imread("20170417212024114.png");
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	Mat thresh_img(img.size(), CV_8UC1);
	threshold(img_gray, thresh_img, 230, 255, THRESH_BINARY_INV);
	imshow("thresh", thresh_img);
	vector<vector<Point>> contours;
	findContours(thresh_img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	RotatedRect mr = minAreaRect(Mat(contours[0]));
	Mat DrawImg(img.size(), img.type(), Scalar(255, 255, 255));
	Point2f vectpoint[4];
	mr.points(vectpoint);
	for (int i = 0; i<4; i++)
	{
		line(DrawImg, vectpoint[i], vectpoint[(i + 1) % 4], Scalar(255, 0, 0), 2);
	}
	imshow("drawimg", DrawImg);
	float angle = 0.0;
	Size si = mr.size;
	if (mr.size.width <= mr.size.height)
	{
		angle = mr.angle + 90;
		swap(si.width, si.height);
	}
	else
	{
		angle = mr.angle;
	}

	Mat rotmat = getRotationMatrix2D(mr.center, angle, 1);
	Mat deal_img;
	warpAffine(img, deal_img, rotmat, img.size(), CV_INTER_CUBIC);
	imshow("deal_img", deal_img);
	Mat rRect;
	getRectSubPix(deal_img, si, mr.center, rRect);
	imshow("截取的矩形区域", rRect);
	waitKey();*/


	Mat img = imread("rectangles.jpg");
	imshow("src_img", img);
	Mat rotmat = getRotationMatrix2D(Point2f(250,250), -30, 1);
	Mat deal_img;
	warpAffine(img, deal_img, rotmat, img.size(), CV_INTER_CUBIC);
	imshow("deal_img", deal_img);
	waitKey();

	return 0;
}

extern int readDir(char *dirName, vector<string> &filesName);
int test36_5(int argc, char * argv[])
{

	string inputPath = "I:/mtcnn-train/rotate";
	string outputPath = "I:/mtcnn-train/rotateResult";
	mkdir(outputPath.c_str());
	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());
	string outputtxt = "I:/mtcnn-train/rotateResult/src/000_one.txt";
	fstream finWrite;
	finWrite.open(outputtxt, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());
	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		cout << v_img_[i] << endl;
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}
		string nametxt = v_img_[i];
		nametxt.replace(nametxt.find_last_of("."), 4, ".txt");

		fstream fin1;
		fin1.open(nametxt);
		if (!fin1.is_open())
		{
			cout << "fin1 文件数据打开错误！" << endl;
			system("pause");
			continue;
		}

		int label; int x1, y1, x2, y2;

		fin1 >> label >> x1 >> y1 >> x2 >> y2;

		fin1.close();
		Point2f center; center.x = (x1 + x2)*1.0 / 2; center.y = (y1+y2)*1.0 / 2;
		int width = x2 - x1; int height = y2 - y1;
		//int  degarr[] = { -90, -80, -70, -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
		int  degarr[] = { -60, -50, -40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, };
		for (int j = 0; j < 13; j++)
		{
			int degree = degarr[j];

			RotatedRect rRect(center, Size2f(width,height), degree);
			Rect brect = rRect.boundingRect();
			Point2f vectpoint[4];
			rRect.points(vectpoint);
			Mat roiSrc = img(brect);

			//Mat rotmat = getRotationMatrix2D(rRect.center, degree, 1);
		
			Mat roiSrct(roiSrc.rows,roiSrc.cols,CV_8UC3);
			roiSrc.copyTo(roiSrct);;
			Mat roiDst = Mat::zeros(roiSrc.rows, roiSrc.cols, CV_8UC3);

			Mat rotmat = getRotationMatrix2D(Point(roiSrc.cols / 2, roiSrc.rows/2), degree, 1);
			warpAffine(roiSrct, roiDst, rotmat, roiSrct.size(), CV_INTER_CUBIC, IPL_BORDER_REPLICATE);
			imshow("1", roiSrc);
			imshow("2", roiDst);
			waitKey();

			/*Mat roiDst;
			Mat img1;
			warpAffine(img, img1, rotmat, img.size(), CV_INTER_CUBIC);
			imshow("1", img);
			imshow("2", img1);
			waitKey();*/

			Mat dst = img.clone();
			Mat ROI = dst(brect);
			roiDst.copyTo(ROI);

			char intstr[128];
			sprintf(intstr, "-%da%d-1", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			Point pt1, pt2;
			pt1.x = brect.x; pt1.y = brect.y; pt2.x = brect.x + brect.width - 1;
			pt2.y = brect.y + brect.height - 1;
			Mat drawimg = dst.clone();
			rectangle(drawimg, pt1, pt2, Scalar(0, 0, 255));
			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);


			float w1 = (vectpoint[0].x - vectpoint[1].x)*(vectpoint[0].x - vectpoint[1].x) + (vectpoint[0].y - vectpoint[1].y)*(vectpoint[0].y - vectpoint[1].y);
			 w1 = sqrt(w1);
			float h1 = (vectpoint[1].x - vectpoint[2].x)*(vectpoint[1].x - vectpoint[2].x) + (vectpoint[1].y - vectpoint[2].y)*(vectpoint[1].y - vectpoint[2].y);
			 h1 = sqrt(h1);

			float angle = degree;
			Size si = rRect.size;
			if (rRect.size.width <= rRect.size.height)
			{
				angle = angle + 90;
				swap(si.width, si.height);
			}
			else
			{
				angle = angle;
			}
			
			Point2f cen; cen.x = brect.x + brect.width / 2; cen.y = brect.y + brect.height / 2;
			RotatedRect dRect(cen, Size2f(w1, h1), angle);
			vector<Point2f> contours(vectpoint, + vectpoint+6 );
			RotatedRect mr = minAreaRect(Mat(contours));
			
			Rect brect1 = dRect.boundingRect();
			Mat roiSrc1 = dst(brect1);
			Mat rotmat1 = getRotationMatrix2D(dRect.center, -degree, 1);
			Mat roiDst1;
			warpAffine(roiSrc1, roiDst1, rotmat1, roiSrc1.size(), CV_INTER_CUBIC);
			Mat dst1 = dst.clone();
			Mat ROI1 = dst1(brect);
			roiDst.copyTo(ROI1);

			Mat dRect1;
			getRectSubPix(dst1, dRect.size, dRect.center, dRect1);
			sprintf(intstr, "-%da%d-3", j, degree);
			str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), dRect1);

			sprintf(intstr, "-%da%d-2", j, degree);
			Point pt3, pt4;
			pt3.x = brect.x; pt3.y = brect.y; pt4.x = brect.x + brect.width - 1;
			pt4.y = brect.y + brect.height - 1;
			rectangle(dst1, pt3, pt4, Scalar(0, 0, 255));
			 str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), dst1);

		}

	


		int jjjjjj = 90;

	}

	finWrite.close();
	Mat img = imread("rectangles.jpg");
	imshow("src_img", img);
	Mat rotmat = getRotationMatrix2D(Point2f(250, 250), -30, 1);
	Mat deal_img;
	warpAffine(img, deal_img, rotmat, img.size(), CV_INTER_CUBIC);
	imshow("deal_img", deal_img);
	waitKey();

	return 0;
}

int test36(int argc, char * argv[])
{
	//test36_1(argc, argv);
	//test36_2(argc, argv);

	//test36_3(argc, argv);

	//test36_4(argc, argv);

	test36_5(argc, argv);

	return 0;
}