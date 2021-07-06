#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <io.h>


#define WINDOWS 1
#define LINUX 0

int LSE = WINDOWS;		// ����ϵͳ����
bool DEBUG = true;		// ����Debugģʽ

using namespace std;
using namespace cv;

// foggy�࣬�������ɴ���ͼ��
class foggy
{
public:
	foggy(Size imageSize)
	{
		// ����ģ��ͼ��
		for (auto A : A_vec)
		{
			vector<vector<Mat>> temp_2d;
			for (auto beta : beta_vec)
			{
				vector<Mat> temp_1d;
				for (auto size : size_vec)
				{
					Mat temp;
					generateTdMat(imageSize, temp, A, beta, size);
					temp_1d.push_back(temp);

					if(DEBUG)
						cout << A << ' ' << beta << ' ' << size << endl;
				}

				temp_2d.push_back(temp_1d);
			}
			td_mat.push_back(temp_2d);
		}
	};

	void generate(vector<string> fileName, string imgType, string loadPath, string savePath);

private:
	vector<double> A_vec = { 0.5, 0.6, 0.7};	// ����
	vector<double> beta_vec = { 0.02, 0.03, 0.04, 0.05};	// Ũ��
	vector<int> size_vec = { 45, 55, 65 };		// �ߴ�

	vector<vector<vector<Mat>>> td_mat;

	void generateTdMat(Size imageSize, Mat &dst, double A, double beta, int size);
};


// ���ɴ���ͼ��
void foggy::generate(vector<string> fileName, string imgType, string loadPath, string savePath)
{
	int imgNum = A_vec.size() * beta_vec.size() * size_vec.size();
	for (auto file:fileName)
	{
		// ��ȡͼƬ
		Mat img_origin = imread(loadPath + "/" + file + imgType, IMREAD_COLOR);
		img_origin.convertTo(img_origin, CV_32FC3, 1.0 / 255.0, 0.0);


		string savePath_now = savePath + "\\" + file;

		if (LSE = WINDOWS)
		{
			// ���Ŀ¼�Ƿ���ڣ��粻���ڣ�������Ŀ¼
			if (_access(savePath_now.data(), 0) == -1)
			{
				string command;
				command = "mkdir " + savePath_now;
				system(command.c_str());
				cout << "create new folder��" << savePath_now << endl;
			}
			else	// ���Ŀ¼���Ƿ����㹻��ͼƬ
			{
				int sum = 0;
				HANDLE h;
				WIN32_FIND_DATA p;
				h = FindFirstFile((savePath_now + "/*.jpg").data(), &p);
				do
				{
					sum++;
				} while (FindNextFile(h, &p));

				// ����У�����
				if (sum == imgNum) {
					cout << "Already exist, skip" << savePath_now << endl;
					continue;
				}
				else
				{
					cout << "Restart generate:" << savePath_now << endl;
				}
			}
		}
		
		for (int i = 0; i < A_vec.size(); i++)
		{
			for (int j = 0; j < beta_vec.size(); j++)
			{
				for (int k = 0; k < size_vec.size(); k++)
				{
					double A = A_vec[i];
					double beta = beta_vec[j];
					int size = size_vec[k];

					// �����ļ���
					char saveFile[80];
					sprintf_s(saveFile, "%s/%s/%s_%.1lf_%.2lf_%d.jpg", savePath.data(), file.data(), file.data(), A, beta, size);
					if(DEBUG)
						cout << saveFile << endl;
					
					// ��ȡ����ͼ��
					Mat img_foggy = img_origin.mul(td_mat[i][j][k]) + A * (1 - td_mat[i][j][k]);
					img_foggy.convertTo(img_foggy, CV_8UC3, 255.0, 0.0);

					// ����ͼ��
					imwrite(saveFile, img_foggy);
				}
			}
		}
	}
}

void foggy::generateTdMat(Size imageSize, Mat &dst, double A, double beta, int size)
{
	int center_width = imageSize.width / 2;
	int center_height = imageSize.height / 2;

	dst = Mat::zeros(imageSize, CV_32FC3);

	for (int i = 0; i < dst.cols; i++)
	{
		for (int j = 0; j < dst.rows; j++)
		{
			double d = -0.04 * sqrt(pow(j - center_height, 2) + pow(i - center_width, 2)) + size;
			double td = exp(-beta * d);
			dst.at<Vec3f>(j, i)[0] = td;
			dst.at<Vec3f>(j, i)[1] = td;
			dst.at<Vec3f>(j, i)[2] = td;
		}
	}
}

int main()
{
	string loadPath = "img";
	string savePath = "foggy";
	string imgType = ".png";

	vector<string> fileName;
	// ��ȡ�ļ���������
	if (LSE == WINDOWS)
	{
		HANDLE h;
		WIN32_FIND_DATA p;
		h = FindFirstFile((loadPath + "/*" + imgType).data(), &p);
		do
		{
			string file = p.cFileName;
			file = file.substr(0, file.rfind('.'));
			fileName.push_back(file);
		} while (FindNextFile(h, &p));
	}

	// ��ȡͼ���С
	Mat img = imread(loadPath + "/" + fileName[0] + imgType, IMREAD_COLOR);
	Size imgSize = Size(img.cols, img.rows);


	foggy foggy_generator(imgSize);

	foggy_generator.generate(fileName, imgType, loadPath, savePath);

	system("Pause");

	return 0;
}