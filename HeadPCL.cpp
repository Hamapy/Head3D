//#pragma once
#pragma warning(disable:4819)
#include "HeadPCL.h"
#include <qprogressdialog.h>
#include <QObject>
#include "qconfig.h"
#include "qdebug.h"

////////////////////////////////公有函数////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// 函数：Head
// 描述：默认构造函数
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Head::Head()
{

}

////////////////////////////////////////////////////////////////////////////
// 函数：~Head
// 描述：默认析构函数
// 输入：Null
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Head::~Head()
{

}

////////////////////////////////////////////////////////////////////////////
// 函数：alignModel
// 描述：对齐点云
// 输入：filePath（stl文件本地路径），cUpRight（摆正后点云指针）
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
pcl::PointCloud<pcl::PointXYZ>::Ptr Head::alignModel(const string filePath, pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight)
{
	////点云文件读取
	//const string filePath(stlFilePath);
	pcl::PolygonMesh mesh;
	pcl::io::loadPolygonFileSTL(filePath, mesh);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cSource(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::fromPCLPointCloud2(mesh.cloud, *cSource);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cHandled(new pcl::PointCloud<pcl::PointXYZ>);

	//progressValue = 1;
	//qApp->processEvents();

	////点云对齐
	//float nVexelSize = 5;
	float nVexelSize = 1;//考虑到校正点取点时偏离的问题
	downSample(cSource, cHandled, nVexelSize);

	//progressValue = 5;
	//qApp->processEvents();

	////减去点云质心
	deMean(cSource, cSource);
	deMean(cHandled, cHandled);

	//progressValue = 10;
	//qApp->processEvents();

	//预处理，绕Z轴逆时针旋转点90度，使人脸竖直显示
	rotateAndTranslate(cHandled, cHandled, M_PI / 2, Eigen::Vector3f(0, 0, 1), Eigen::Vector3f(0, 0, 0));
	//预处理，绕y轴逆时针旋转点180度，使人脸朝我们
	rotateAndTranslate(cHandled, cHandled, M_PI, Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 0, 0));

	//progressValue = 15;
	//qApp->processEvents();

	//使人脸点云摆正
	bool depart = true;
	//bool depart = false;
	bool corsetofine = true;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRightTemp1(new pcl::PointCloud<pcl::PointXYZ>);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRightTemp1_v2(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRightTemp2(new pcl::PointCloud<pcl::PointXYZ>);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRightTemp2_v2(new pcl::PointCloud<pcl::PointXYZ>);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight(new pcl::PointCloud<pcl::PointXYZ>);
	float nBestTheta_x, nBestTheta_y, nBestTheta_z;
	std::tuple<float, float, float> nBestTheta_xyz;

	float nGoodTheta_x, nGoodTheta_z, nGoodTheta_y;

	//progressValue = 20;
	//qApp->processEvents();

	if (depart)
	{
		if (corsetofine)
		{
			//使点云绕X轴旋转投影XZ平面后的面积最小
			nGoodTheta_x = searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(cHandled, Eigen::Vector3f(1, 0, 0), nVexelSize, cUpRightTemp1, 0, M_PI / 9, 3 * M_PI / 180);
			nBestTheta_x = searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(cHandled, Eigen::Vector3f(1, 0, 0), nVexelSize, cUpRightTemp1, nGoodTheta_x, 3 * M_PI / 180, M_PI / 180);

			//使点云绕Z轴旋转投影XZ平面后的面积最小
			nGoodTheta_z = searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(cUpRightTemp1, Eigen::Vector3f(0, 0, 1), nVexelSize, cUpRightTemp2, 0, M_PI / 9, 3 * M_PI / 180);
			nBestTheta_z = searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(cUpRightTemp1, Eigen::Vector3f(0, 0, 1), nVexelSize, cUpRightTemp2, nGoodTheta_z, 3 * M_PI / 180, M_PI / 180);

			//使点云绕Y轴旋转根据YZ平面对称
			nGoodTheta_y = searchBestRotationAngleForSymmetry_V2(cUpRightTemp2, Eigen::Vector3f(0, 1, 0), Eigen::Vector4f(1, 0, 0, 0), cUpRight, 0, M_PI / 9, 3 * M_PI / 180);
			nBestTheta_y = searchBestRotationAngleForSymmetry_V2(cUpRightTemp2, Eigen::Vector3f(0, 1, 0), Eigen::Vector4f(1, 0, 0, 0), cUpRight, nGoodTheta_y, 3 * M_PI / 180, M_PI / 180);
		}
		else
		{
			//使点云绕X轴旋转投影XZ平面后的面积最小
			nBestTheta_x = searchBestRotationAngleForSmallestAreaOnPlaneXZ(cHandled, Eigen::Vector3f(1, 0, 0), nVexelSize, cUpRightTemp1);

			//使点云绕Z轴旋转投影XZ平面后的面积最小
			nBestTheta_z = searchBestRotationAngleForSmallestAreaOnPlaneXZ(cUpRightTemp1, Eigen::Vector3f(0, 0, 1), nVexelSize, cUpRightTemp2);

			//使点云绕Y轴旋转根据YZ平面对称
			nBestTheta_y = searchBestRotationAngleForSymmetry(cUpRightTemp2, Eigen::Vector3f(0, 1, 0), Eigen::Vector4f(1, 0, 0, 0), cUpRight);
		}
	}
	else
	{
		//使点云同时绕XYZ轴旋，投影XZ平面后的面积最小，且根据YZ平面对称
		nBestTheta_xyz = searchBestXYZRotationAngleForUpRight(cHandled, cUpRight, nVexelSize);
	}

	//progressValue = 25;
	//qApp->processEvents();

	if (false)
	{
		//旋转和平移点云
		pcl::PointCloud<pcl::PointXYZ>::Ptr cAffined(new pcl::PointCloud<pcl::PointXYZ>);
		Eigen::Vector3f axis(0, 0, 1);
		Eigen::Vector3f displacement(0, 0, 0);

		float theta = M_PI / 4;
		rotateAndTranslate(cHandled, cAffined, theta, axis, displacement);

		//投影点云到平面
		pcl::PointCloud<pcl::PointXYZ>::Ptr cProjected(new pcl::PointCloud<pcl::PointXYZ>);
		Eigen::Vector4f coefficients(0, 0, 1, 0);
		projectToPlane(cHandled, cProjected, coefficients);

		//切割点云截面
		float nThreshold = 5;
		pcl::PointCloud<pcl::PointXYZ>::Ptr cCrossSection(new pcl::PointCloud<pcl::PointXYZ>);
		sliceToPlane(cHandled, cCrossSection, coefficients, nThreshold);

		//获取点云边界
		Eigen::Matrix<float, 6, 1> vBoundBoxSize;
		vBoundBoxSize = getBoundBoxSize(cHandled);
	}

	//progressValue = 30;
	//qApp->processEvents();

	return cUpRight;
}

////////////////////////////////////////////////////////////////////////////
// 函数：restruction
// 描述：显示点云模型
// 输入：cUpRight（摆正后点云指针）
// 输出：Null
// 返回：mesh（点云表面网格）
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
PolygonMesh Head::restruction(pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight)
{
	////曲面重建
	//法向计算
	pcl::NormalEstimationOMP<PointXYZ, Normal> ne;
	ne.setNumberOfThreads(8);
	ne.setInputCloud(cUpRight);
	ne.setRadiusSearch(5);
	Eigen::Vector4f centroid;
	compute3DCentroid(*cUpRight, centroid);
	ne.setViewPoint(centroid[0], centroid[1], centroid[2]);
	PointCloud<Normal>::Ptr cloud_normals(new PointCloud<Normal>());
	ne.compute(*cloud_normals);

	for (size_t i = 0; i < cloud_normals->size(); ++i) {
		cloud_normals->points[i].normal_x *= -1;
		cloud_normals->points[i].normal_y *= -1;
		cloud_normals->points[i].normal_z *= -1;
	}
	PointCloud<PointNormal>::Ptr cloud_smoothed_normals(new PointCloud<PointNormal>());
	concatenateFields(*cUpRight, *cloud_normals, *cloud_smoothed_normals);//将点云数据的坐标和法向信息拼接

	Poisson<PointNormal> poisson;
	// poisson.setDepth(9);
	poisson.setInputCloud(cloud_smoothed_normals);
	PolygonMesh mesh;
	poisson.reconstruct(mesh);

	return mesh;
}

////////////////////////////////////////////////////////////////////////////
// 函数：showModel
// 描述：显示点云
// 输入：mesh（点云表面网格），viewer（可视化窗口指针）
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::showModel(PolygonMesh mesh, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer)
{
	viewer->removePolygonMesh("mesh");
	viewer->addPolygonMesh(mesh, "mesh");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0.5, 0.8, 1, "mesh");
	viewer->addCoordinateSystem(20.0, "Coordinate");
}

////////////////////////////////////////////////////////////////////////////
// 函数：showKeyPoints
// 描述：在点云窗口显示关键点
// 输入：keyPtr（关键点点云指针），viewer（可视化窗口指针）
// 输出：Null
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::showKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer)
{
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> red(keyPtr, 255, 0, 0);//红色																					
	//viewer->updatePointCloud(keyPtr, red, "cloud");
	viewer->removePointCloud("cloud");
	viewer->addPointCloud(keyPtr, red, "cloud");
	viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 7, "cloud");
}

////////////////////////////////////////////////////////////////////////////
// 函数：getKeyPoints
// 描述：获取关键点
// 输入：keyPtr（待校正关键点点云指针），cInput（摆正后点云指针）
// 输出：
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
pcl::PointCloud<pcl::PointXYZ>::Ptr Head::getKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, pcl::PointCloud<pcl::PointXYZ>::Ptr cInput)
{
	pcl::PointXYZ minPt, maxPt;
	pcl::getMinMax3D(*cInput, minPt, maxPt);//获取原始点云x,y,z的最值
	pcl::PointCloud<pcl::PointXYZ>::Ptr cCut(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cTemp(new pcl::PointCloud<pcl::PointXYZ>);
	keyPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);

	pcl::PointXYZ nosePt;
	pcl::PointXYZ headTopPt;

	////////////////鼻尖点/////////////////////////
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (cInput->points.at(i).z == maxPt.z)
		{
			nosePt.x = cInput->points.at(i).x;
			nosePt.y = cInput->points.at(i).y;
			nosePt.z = cInput->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(nosePt);

	////////////////头顶点////////////////////////
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (cInput->points.at(i).y == maxPt.y)
		{
			headTopPt.x = cInput->points.at(i).x;
			headTopPt.y = cInput->points.at(i).y;
			headTopPt.z = cInput->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(headTopPt);

	//////////////////后脑最高点////////////////
	cTemp->clear();
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (cInput->points.at(i).y > 0)
		{
			cTemp->points.push_back(cInput->points.at(i));
		}
	}

	pcl::PointXYZ afterBrainPt;
	pcl::getMinMax3D(*cTemp, minPt, maxPt);
	for (int i = 0; i < cTemp->points.size(); i++)
	{
		if (cTemp->points.at(i).z == minPt.z)
		{
			afterBrainPt.x = cTemp->points.at(i).x;
			afterBrainPt.y = cTemp->points.at(i).y;
			afterBrainPt.z = cTemp->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(afterBrainPt);

	///////////////////下颚点////////////////////
	pcl::PointXYZ jawPt, pCenter;
	Eigen::Vector4f pCentroid;
	pcl::compute3DCentroid(*cInput, pCentroid);
	pCenter.x = pCentroid[0];
	pCenter.y = pCentroid[1];
	pCenter.z = pCentroid[2];

	cTemp->clear();
	float d;
	float dmax = 0;
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if ((cInput->points.at(i).y) < 0 && (cInput->points.at(i).z  > (0.6 * nosePt.z)))
		{
			cTemp->points.push_back(cInput->points.at(i));
			d = computeDistance3D(headTopPt, cInput->points.at(i));
			if (dmax < d) dmax = d;
		}
	}

	for (int i = 0; i < cTemp->points.size(); i++)
	{
		d = computeDistance3D(headTopPt, cTemp->points.at(i));
		if (d == dmax)
		{
			jawPt.x = cTemp->points.at(i).x;
			jawPt.y = cTemp->points.at(i).y;
			jawPt.z = cTemp->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(jawPt);

	///////////////山根点（眉间点不明显）////////////
	cTemp->clear();
	cCut->clear();
	pcl::PointXYZ noseRootPt;
	Eigen::Vector4f coefficients(1, 0, 0, 0);
	sliceToPlane(cInput, cCut, coefficients, 1);

	for (int i = 0; i < cCut->points.size(); i++)
	{
		if ((cCut->points.at(i).y > (nosePt.y + 20)) && (cCut->points.at(i).y < headTopPt.y) && (cCut->points.at(i).z > 0))
			cTemp->points.push_back(cCut->points.at(i));
	}

	for (int i = 3; i < cTemp->points.size() - 4; i++)//取3容错率稍微大一点，算法还是不鲁棒
	{
		float flag = (cTemp->points.at(i - 3).z - cTemp->points.at(i).z) * (cTemp->points.at(i).z - cTemp->points.at(i + 3).z);
		if (flag < 0)
		{
			noseRootPt.x = cTemp->points.at(i).x;
			noseRootPt.y = cTemp->points.at(i).y;
			noseRootPt.z = cTemp->points.at(i).z;
			break;
		}
	}

	////////////////////眉间点////////////////////
	pcl::PointXYZ glabellumPt;
	cTemp->clear();
	for (int i = 0; i < cCut->points.size(); i++)
	{
		if ((cCut->points.at(i).y > noseRootPt.y) && (cCut->points.at(i).y < headTopPt.y))
			cTemp->points.push_back(cCut->points.at(i));
	}

	pcl::getMinMax3D(*cTemp, minPt, maxPt);
	for (int i = 0; i < cTemp->points.size(); i++)
	{
		if (cTemp->points.at(i).z == maxPt.z)
		{
			glabellumPt.x = cTemp->points.at(i).x;
			glabellumPt.y = cTemp->points.at(i).y;
			glabellumPt.z = cTemp->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(glabellumPt);

	//////////////////////耳朵上下端点////////////////////
	pcl::PointXYZ earTopPt, earBottomPt;
	pcl::PointXYZ cutTop, cutBottom;
	float tempTop = 0;
	float tempBottom = 0;
	pcl::getMinMax3D(*cInput, minPt, maxPt);
	for (float i = maxPt.x; i > 0; i -= 0.5)
	{
		cCut->clear();
		Eigen::Vector4f coefficients_zy(1, 0, 0, -i);
		sliceToPlane(cInput, cCut, coefficients_zy, 1);
		pcl::getMinMax3D(*cCut, cutBottom, cutTop);
		if ((cutTop.y - tempTop) < 0)
		{
			earTopPt.x = cutTop.x;
			earTopPt.y = cutTop.y;
			earTopPt.z = cutTop.z;
			keyPtr->points.push_back(earTopPt);
			break;
		}
		else
			tempTop = cutTop.y;
	}


	cCut->clear();
	Eigen::Vector4f coefficients_zy(1, 0, 0, -earTopPt.x);
	sliceToPlane(cInput, cCut, coefficients_zy, 1);
	pcl::getMinMax3D(*cCut, cutBottom, cutTop);
	earBottomPt.x = cutBottom.x;
	earBottomPt.y = cutBottom.y;
	earBottomPt.z = cutBottom.z;
	keyPtr->points.push_back(earBottomPt);

	/*for (float i = earTopPt.x; i > 0; i -= 0.5)
	{
		cCut->clear();
		Eigen::Vector4f coefficients_zy(1, 0, 0, -i);
		sliceToPlane(cInput, cCut, coefficients_zy, 1);
		pcl::getMinMax3D(*cCut, cutBottom, cutTop);
		if ((cutBottom.y - tempBottom) > 0)
		{
			earBottomPt.x = cutBottom.x;
			earBottomPt.y = cutBottom.y;
			earBottomPt.z = cutBottom.z;
			keyPtr->points.push_back(earBottomPt);
			break;
		}
		else
			tempBottom = cutBottom.y;
	}*/

	////////////////////脸宽两点////////////////////
	pcl::PointXYZ faceWidthPt_1, faceWidthPt_2;
	cTemp->clear();
	cCut->clear();
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (cInput->points.at(i).z >(nosePt.z / 3.0))
			cTemp->points.push_back(cInput->points.at(i));
	}
	float dd = (noseRootPt.y + nosePt.y) / 2.0;//按比例定义
	Eigen::Vector4f coefficients_zx(0, 1, 0, -dd);
	sliceToPlane(cTemp, cCut, coefficients_zx, 2);
	pcl::getMinMax3D(*cCut, minPt, maxPt);

	for (int i = 0; i < cCut->points.size(); i++)
	{
		if (cCut->points.at(i).x == maxPt.x)
		{
			faceWidthPt_1.x = cCut->points.at(i).x;
			faceWidthPt_1.y = cCut->points.at(i).y;
			faceWidthPt_1.z = cCut->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(faceWidthPt_1);

	for (int i = 0; i < cCut->points.size(); i++)
	{
		if (cCut->points.at(i).x == minPt.x)
		{
			faceWidthPt_2.x = cCut->points.at(i).x;
			faceWidthPt_2.y = cCut->points.at(i).y;
			faceWidthPt_2.z = cCut->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(faceWidthPt_2);

	/////////////////////下颚宽两点（按比例）///////////////////
	pcl::PointXYZ jawWidthPt_1, jawWidthPt_2;
	cTemp->clear();
	cCut->clear();
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (cInput->points.at(i).z >(nosePt.z / 3.0))
			cTemp->points.push_back(cInput->points.at(i));
	}
	float dd_1 = nosePt.y - (nosePt.y - jawPt.y) / 2.0;//按比例定义下颚区域
	Eigen::Vector4f coefficients_zx1(0, 1, 0, -dd_1);
	sliceToPlane(cTemp, cCut, coefficients_zx1, 1);
	pcl::getMinMax3D(*cCut, minPt, maxPt);

	for (int i = 0; i < cCut->points.size(); i++)
	{
		if (cCut->points.at(i).x == maxPt.x)
		{
			jawWidthPt_1.x = cCut->points.at(i).x;
			jawWidthPt_1.y = cCut->points.at(i).y;
			jawWidthPt_1.z = cCut->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(jawWidthPt_1);

	for (int i = 0; i < cCut->points.size(); i++)
	{
		if (cCut->points.at(i).x == minPt.x)
		{
			jawWidthPt_2.x = cCut->points.at(i).x;
			jawWidthPt_2.y = cCut->points.at(i).y;
			jawWidthPt_2.z = cCut->points.at(i).z;
			break;
		}
	}
	keyPtr->points.push_back(jawWidthPt_2);

	////////////////伪额头最高点点（质点）////////////////////
	pcl::PointXYZ newPt;
	pcl::compute3DCentroid(*cInput, pCentroid);
	newPt.x = pCentroid[0];
	newPt.y = pCentroid[1];
	newPt.z = pCentroid[2];
	keyPtr->points.push_back(newPt);//一共12个点

	return keyPtr;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeHeadCircumference
// 描述：计算头围长度
// 输入：cInput（输入点云）、glabellumPt（眉心点）
// 输出：
// 返回：length（头围长度）
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeHeadCircumference(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointXYZ glabellumPt)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cCut(new pcl::PointCloud<pcl::PointXYZ>);
	Eigen::Vector4f coefficients(0, 1, 0, -glabellumPt.y);//定义头围切面
	sliceToPlane(cInput, cCut, coefficients, 2);
	Eigen::Vector4f pCentroid;
	pcl::compute3DCentroid(*cCut, pCentroid);
	pcl::PointXYZ pCenter;
	pCenter.x = pCentroid[0];
	pCenter.y = pCentroid[1];
	pCenter.z = pCentroid[2];
	float length = 0;
	const float theta_T = 3;
	const int m = 360 / theta_T;
	vector<float> r;
	vector<int> index;
	vector<float> R;
	vector<float> cosr;
	for (int i = 0; i < cCut->points.size(); i++)
	{
		float a[2] = { (cCut->points.at(i).x - pCenter.x),(cCut->points.at(i).z - pCenter.z) };
		//float b[2] = { (cCut->points.at(i).x - pCenter.x),(cCut->points.at(i).z - pCenter.z) };

		float angle = computeAngle(0, 0, a[1], a[2]);
		//float angle1 = computeAngle(0, 0, b[1], b[2]);
		//float ab = a[0] * b[0] + a[1] * b[1];
		//float a1 = sqrt(a[0] * a[0] + a[1] * a[1]);
		//float b1 = sqrt(b[0] * b[0] + b[1] * b[1]);
		//cosr.push_back(ab / a1 / b1);
		float theta = angle * 180 / M_PI;
		index.push_back(floor(theta / theta_T));
		float RR = powf((cCut->points.at(i).x - pCenter.x), 2) + powf((cCut->points.at(i).z - pCenter.z), 2);
		R.push_back(sqrtf(RR));
	}

	for (int j = 0; j < m; j++)
	{
		float n = 0;
		float rr = 0;
		for (int i = 0; i < cCut->points.size(); i++)
		{
			if (index.at(i) == j)
			{
				n++;
				rr += R.at(i);
			}
			else n = 1;//防止inf
		}
		r.push_back(rr / n);
		length += M_PI * 2 * r.at(j) * theta_T / 360.00;
	}

	return length;
}



////////////////////////////////私有函数////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// 函数：computeKeySize
// 描述：计算关键尺寸
// 输入：原始关键点点云
// 输出：Null
// 返回：关键尺寸数组指针
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
//const char* Head::computeKeySize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, int keyOrder)
//{
//	int size0 = computeHeadCircumference(cInput, keyPtr->points.at(4));//头围
//	int size1 = fabs(keyPtr->points.at(1).y - keyPtr->points.at(3).y);//头顶到下颚距离
//	int size2 = fabs(keyPtr->points.at(7).x - keyPtr->points.at(8).x);//脸宽
//	int size3 = fabs(keyPtr->points.at(9).x - keyPtr->points.at(10).x);//下颚宽度
//	int size4 = computeDistance3D(keyPtr->points.at(4), keyPtr->points.at(0));//眉心到鼻下缘距离
//	int size5 = fabs(keyPtr->points.at(0).z - keyPtr->points.at(4).z);//眉心到鼻下缘水平距离
//	int size6 = fabs(keyPtr->points.at(5).y - keyPtr->points.at(6).y);//耳长
//	int size7 = fabs(keyPtr->points.at(0).z - keyPtr->points.at(2).z);//后脑最高点到鼻尖距离
//	int size8 = fabs(keyPtr->points.at(4).z - keyPtr->points.at(2).z);//眉骨到后脑最高点距离
//
//	const char* keyChar;
//	if (keyOrder != 12)
//	{
//		stringstream Size0, Size1, Size2, Size3, Size4, Size5, Size6, Size7, Size8;
//		Size0 << size0;
//		Size1 << size1;
//		Size2 << size2;
//		Size3 << size3;
//		Size4 << size4;
//		Size5 << size5;
//		Size6 << size6;
//		Size7 << size7;
//		Size8 << size8;
//
//		string s0 = Size0.str();//提取字符串
//		string s1 = Size1.str();
//		string s2 = Size2.str();
//		string s3 = Size3.str();
//		string s4 = Size4.str();
//		string s5 = Size5.str();
//		string s6 = Size6.str();
//		string s7 = Size7.str();
//		string s8 = Size8.str();
//		string keyString = s0 + " "
//			+ s1 + " "
//			+ s2 + " "
//			+ s3 + " "
//			+ s4 + " "
//			+ s5 + " "
//			+ s6 + " "
//			+ s7 + " "
//			+ s8;
//
//		keyChar = keyString.c_str();
//	}
//	else
//	{
//		int size9 = fabs(keyPtr->points.at(11).z - keyPtr->points.at(2).z);//后脑最高点到额头最高点距离
//		int size10 = computeHeadCircumference(cInput, keyPtr->points.at(11));//头围//额头最高点处头围
//		stringstream Size0, Size1, Size2, Size3, Size4, Size5, Size6, Size7, Size8, Size9, Size10;
//		Size0 << size0;
//		Size1 << size1;
//		Size2 << size2;
//		Size3 << size3;
//		Size4 << size4;
//		Size5 << size5;
//		Size6 << size6;
//		Size7 << size7;
//		Size8 << size8;
//		Size9 << size9;
//		Size10 << size10;
//
//		string s0 = Size0.str();//提取字符串
//		string s1 = Size1.str();
//		string s2 = Size2.str();
//		string s3 = Size3.str();
//		string s4 = Size4.str();
//		string s5 = Size5.str();
//		string s6 = Size6.str();
//		string s7 = Size7.str();
//		string s8 = Size8.str();
//		string s9 = Size9.str();
//		string s10 = Size10.str();
//		string keyString = s0 + " "
//			+ s1 + " "
//			+ s2 + " "
//			+ s3 + " "
//			+ s4 + " "
//			+ s5 + " "
//			+ s6 + " "
//			+ s7 + " "
//			+ s8 + " "
//			+ s9 + " "
//			+ s10;
//
//		keyChar = keyString.c_str();
//	}
//
//	return keyChar;
//}

////////////////////////////////////////////////////////////////////////////
// 函数：computeDistance3D
// 描述：计算两点的三维距离
// 输入：pA(点A)，pB(点B)
// 输出：Null
// 返回：dist(距离)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeDistance3D(pcl::PointXYZ pA, pcl::PointXYZ pB)
{
	float xv = pA.x - pB.x;
	float yv = pA.y - pB.y;
	float zv = pA.z - pB.z;
	float dist = sqrt(xv*xv + yv * yv + zv * zv);

	return dist;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeDistance2d
// 描述：计算2点的二维距离
// 输入：pA(点A)，pB(点B)
// 输出：Null
// 返回：dist（距离）
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeDistance2D(pcl::PointXYZ pointA, pcl::PointXYZ pointB)
{
	double dist;
	dist = powf((pointA.x - pointB.x), 2) + powf((pointA.y - pointB.y), 2);
	dist = sqrtf(dist);

	return dist;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeTriangleArea
// 描述：计算三点的面积
// 输入：pA(点A)，pB(点B)，pC(点C)
// 输出：Null
// 返回： (面积)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeTriangleArea(pcl::PointXYZ pA, pcl::PointXYZ pB, pcl::PointXYZ pC)
{
	float AB = computeDistance3D(pA, pB);
	float BC = computeDistance3D(pB, pC);
	float AC = computeDistance3D(pA, pC);
	float s = (AB + BC + AC) / 2;
	return sqrt(s*(s - AB)*(s - BC)*(s - AC));
}


////////////////////////////////////////////////////////////////////////////
// 函数：computePointToPlaneDistance
// 描述：计算点到一指定平面的垂直距离
// 输入：cInput(输入点云)，coefficients(平面系数：ax+by+cz+d=0)
// 输出：cOutput(输出点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computePointToPlaneDistance(pcl::PointXYZ pA, Eigen::Vector4f cPlane)
{
	float dist = fabs(pA.x*cPlane[0] + pA.y*cPlane[1] + pA.z*cPlane[2] + cPlane[3]) / sqrt(cPlane[0] * cPlane[0] + cPlane[1] * cPlane[1] + cPlane[2] * cPlane[2]);
	return dist;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeSymmetryScore
// 描述：计算点云对于一个面的对称性分数
// 输入：cInput(输入点云)、coefficients(平面系数：ax+by+cz+d=0)
// 输出：Null
// 返回：nScore(对称性指标：值越小，对称性越好)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeSymmetryScore(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f coefficients)
{

	pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
	kdtree.setInputCloud(cInput);

	float Ox, Oy, Oz;
	if (coefficients[0] != 0)
	{
		Ox = -coefficients[3] / coefficients[0]; Oy = 0; Oz = 0;
	}
	else if (coefficients[1] != 0)
	{
		Ox = 0; Oy = -coefficients[3] / coefficients[1]; Oz = 0;
	}
	else
	{
		Ox = 0; Oy = 0;  Oz = -coefficients[3] / coefficients[2];
	}

	Eigen::Vector3f pOrigin(Ox, Oy, Oz), pNormal(coefficients[0], coefficients[1], coefficients[2]);
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cMirrored;

	float nScore = 0;  //越小越好
	Eigen::Vector3f pSelected, pProjected;

	pcl::PointXYZ pMirrored, pClosest;

	for (int i = 0; i < cInput->points.size(); ++i)
	{

		pSelected = Eigen::Vector3f(cInput->points[i].x, cInput->points[i].y, cInput->points[i].z);;

		pcl::geometry::project(pSelected, pOrigin, pNormal, pProjected);   //将点投影到面
		pMirrored.x = -pSelected[0] + 2 * pProjected[0];
		pMirrored.y = -pSelected[1] + 2 * pProjected[1];
		pMirrored.z = -pSelected[2] + 2 * pProjected[2];
		//cMirrored->push_back(pMirrored);

		int K = 1;  //从原始点云cInput中找该镜像点pMirrored最近的1个点
		std::vector<int> pointIdxNKNSearch(K);
		std::vector<float> pointNKNSquaredDistance(K);

		kdtree.nearestKSearch(pMirrored, K, pointIdxNKNSearch, pointNKNSquaredDistance);
		//pClosest = cInput->points[pointIdxNKNSearch[0]];
		nScore += sqrt(pointNKNSquaredDistance[0]);

	}
	nScore /= cInput->points.size();

	return nScore;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computerVexelNumOfPlaneXZ
// 描述：计算投影面体素化后的个数，投影面在XZ平面上
// 输入：cInputPlane(输入XZ平面点云)、nScale(体素化的尺寸)
// 输出：Null
// 返回：nScore(对称性指标：值越小，对称性越好)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
int Head::computerVexelNumOfPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInputPlane, float nScale)
{
	pcl::VoxelGrid<pcl::PointXYZ> sor;
	pcl::PointCloud<pcl::PointXYZ>::Ptr cVexeled(new pcl::PointCloud<pcl::PointXYZ>);
	sor.setInputCloud(cInputPlane);
	sor.setLeafSize(nScale, nScale, nScale);
	sor.filter(*cVexeled);
	int nNum = (int)cVexeled->size();

	return nNum;
}

////////////////////////////////////////////////////////////////////////////
// 函数：getBoundBoxSize
// 描述：得到点云的xmin,xmax,ymin,ymax,zmin,zmax坐标
// 输入：cInput(输入点云)
// 输出：Null
// 返回：点云临界尺寸坐标
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
Eigen::Matrix<float, 6, 1> Head::getBoundBoxSize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput)
{
	Eigen::Matrix<float, 6, 1> vBoundBoxSize;
	float nDist = 0;
	for (int i = 0; i < cInput->points.size(); i++)
	{
		if (i == 0)
		{
			vBoundBoxSize[0] = cInput->points.at(i).x;
			vBoundBoxSize[1] = cInput->points.at(i).x;
			vBoundBoxSize[2] = cInput->points.at(i).y;
			vBoundBoxSize[3] = cInput->points.at(i).y;
			vBoundBoxSize[4] = cInput->points.at(i).z;
			vBoundBoxSize[5] = cInput->points.at(i).z;
		}
		else
		{
			if (cInput->points.at(i).x < vBoundBoxSize[0])
			{
				vBoundBoxSize[0] = cInput->points.at(i).x;
			}
			if (cInput->points.at(i).x > vBoundBoxSize[1])
			{
				vBoundBoxSize[1] = cInput->points.at(i).x;
			}
			if (cInput->points.at(i).y < vBoundBoxSize[2])
			{
				vBoundBoxSize[2] = cInput->points.at(i).y;
			}
			if (cInput->points.at(i).y > vBoundBoxSize[3])
			{
				vBoundBoxSize[3] = cInput->points.at(i).y;
			}
			if (cInput->points.at(i).z < vBoundBoxSize[4])
			{
				vBoundBoxSize[4] = cInput->points.at(i).z;
			}
			if (cInput->points.at(i).z > vBoundBoxSize[5])
			{
				vBoundBoxSize[5] = cInput->points.at(i).z;
			}
		}
	}
	return vBoundBoxSize;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeAngle
// 描述：计算向量夹角
// 输入：两点坐标
// 输出：Null
// 返回：角度
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeAngle(float x1, float y1, float x2, float y2)
{
	float angle_temp;
	float xx, yy;

	xx = x2 - x1;
	yy = y2 - y1;

	if (xx == 0.0)
		angle_temp = M_PI / 2.0;
	else
		angle_temp = atan(fabs(yy / xx));

	if ((xx < 0.0) && (yy >= 0.0))
		angle_temp = M_PI - angle_temp;
	else if ((xx < 0.0) && (yy < 0.0))
		angle_temp = M_PI + angle_temp;
	else if ((xx >= 0.0) && (yy < 0.0))
		angle_temp = M_PI * 2.0 - angle_temp;

	return angle_temp;
}

////////////////////////////////////////////////////////////////////////////
// 函数：downsample
// 描述：对点云进行降采样，每个体素的重心为采样点
// 输入：cInput(待采样点云)、nScale(降采样体素的尺寸，mm)
// 输出：cOutput(采样后点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::downSample(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale)
{
	pcl::VoxelGrid<pcl::PointXYZ> sor;
	sor.setInputCloud(cInput);
	sor.setLeafSize(nScale, nScale, nScale);
	sor.filter(*cOutput);
}

////////////////////////////////////////////////////////////////////////////
// 函数：deMean
// 描述：对点云的质心移到（0,0,0）
// 输入：cInput(输入点云)
// 输出：cOutput(输入点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::deMean(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput)
{
	Eigen::Vector4f pCentroid;
	pcl::compute3DCentroid(*cInput, pCentroid);
	pcl::demeanPointCloud(*cInput, pCentroid, *cOutput);
}

////////////////////////////////////////////////////////////////////////////
// 函数：scaleTransform
// 描述：对点云进行尺度变换
// 输入：cInput(输入点云)
// 输出：cOutput(输入点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::scaleTransform(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float scale)
{
	for (int i = 0; i < cInput->points.size(); i++)
	{
		pcl::PointXYZ p;
		p.x = cInput->points.at(i).x / scale;
		p.y = cInput->points.at(i).y / scale;
		p.z = cInput->points.at(i).z / scale;
		cOutput->points.push_back(p);
	}
}

////////////////////////////////////////////////////////////////////////////
// 函数：rotateAndTranslate
// 描述：对点云进行旋转和平移
// 输入：cInput(输入点云)，theta(旋转的角度)，axis(旋转轴的方向向量)、displacement(平移向量)
// 输出：cOutput(输出点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::rotateAndTranslate(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta, Eigen::Vector3f axis, Eigen::Vector3f displacement)
{
	Eigen::Affine3f mTransform = Eigen::Affine3f::Identity();
	mTransform.translation() << displacement[0], displacement[1], displacement[2];
	mTransform.rotate(Eigen::AngleAxisf(theta, Eigen::Vector3f(axis[0], axis[1], axis[2])));    //沿轴旋转
	pcl::transformPointCloud(*cInput, *cOutput, mTransform);
}


////////////////////////////////////////////////////////////////////////////
// 函数：projectToPlane
// 描述：对点云投影到一指定平面上
// 输入：cInput(输入点云)，coefficients(平面系数：ax+by+cz+d=0)
// 输出：cOutput(输出点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::projectToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients)
{
	pcl::ModelCoefficients::Ptr cPlane(new pcl::ModelCoefficients());
	cPlane->values.resize(4);
	cPlane->values[0] = coefficients[0];
	cPlane->values[1] = coefficients[1];
	cPlane->values[2] = coefficients[2];
	cPlane->values[3] = coefficients[3];

	pcl::ProjectInliers<pcl::PointXYZ> proj;
	proj.setModelType(pcl::SACMODEL_PLANE);
	proj.setInputCloud(cInput);
	proj.setModelCoefficients(cPlane);
	proj.filter(*cOutput);
}


////////////////////////////////////////////////////////////////////////////
// 函数：sliceToPlane
// 描述：截取点云一指定截面
// 输入：cInput(输入点云)，coefficients(截平面系数：ax+by+cz+d=0)
// 输出：cOutput(输出点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::sliceToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, float nThreshold)
{
	cOutput->clear();

	pcl::PointXYZ pSelect;
	float nDist = 0;
	for (int i = 0; i < cInput->points.size(); i++)
	{
		pSelect = cInput->points.at(i);
		nDist = computePointToPlaneDistance(pSelect, coefficients);
		if (nDist < nThreshold)
		{
			cOutput->points.push_back(pSelect);
		}
	}

	projectToPlane(cOutput, cOutput, coefficients);
}

////////////////////////////////////////////////////////////////////////////
// 函数：upSamplingInVexel
// 描述：对点云进行上采样，以指定体素尺寸为单位（VOXEL_GRID_DILATION）
// 输入：cInput(输入点云)，nScale(体素的尺寸:mm)
// 输出：cOutput(输出点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::upSamplingInVexel(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale)
{
	pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointXYZ> filter;  //滤波对象
	filter.setInputCloud(cInput);

	pcl::search::KdTree<pcl::PointXYZ>::Ptr kdtree; //建立搜索对象
	filter.setSearchMethod(kdtree);

	filter.setSearchRadius(20);  //设置搜索邻域的半径为20mm    
	filter.setUpsamplingMethod(pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointXYZ>::VOXEL_GRID_DILATION);  // Upsampling 采样的方法还有 DISTINCT_CLOUD, RANDOM_UNIFORM_DENSITY，SAMPLE_LOCAL_PLANE，该方法主要用于表面填补
	filter.setDilationIterations(3);  //设定迭代次数
	filter.setDilationVoxelSize(nScale);  //设定voxel的大小

	filter.process(*cOutput);
}

////////////////////////////////////////////////////////////////////////////
// 函数：searchBestRotationAngle
// 描述：寻找最佳的旋转角度，使旋转后点云在某个平面的对称性最佳
// 输入：cInput(输入点云)，axis(旋转轴），cPlane（对称平面系数）
// 输出：cOutput(输出点云)
// 返回：nBestTheta(最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::searchBestRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput)
{
	float nBestScore = 1000;
	float nScore;
	float nBestTheta = 2 * M_PI;
	Eigen::Matrix<float, 6, 1> vBound;
	//time_t clockBegin, clockEnd;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudFront(new pcl::PointCloud<pcl::PointXYZ>);

	for (float theta = -M_PI / 9; theta < M_PI / 9; theta += M_PI / 180)
	{

		rotateAndTranslate(cInput, cAuxCloud, theta, axis, Eigen::Vector3f(0, 0, 0));  //绕轴旋转
		vBound = getBoundBoxSize(cAuxCloud);
		cAuxCloudFront->clear();
		/*if (axis == Eigen::Vector3f(0, 0, 1)) //提取人头Y轴前1/4处的点
		{
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
		if (cAuxCloud->points[i].y > vBound(3, 0)*0.25)
		{
		cAuxCloudFront->push_back(cAuxCloud->points[i]);
		}
		}
		}
		else  //提取人头Z轴前1/4处的点
		{
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
		if (cAuxCloud->points[i].z > vBound(5, 0)*0.25)
		{
		cAuxCloudFront->push_back(cAuxCloud->points[i]);
		}
		}
		}*/
		//clockBegin = clock();
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
			if (cAuxCloud->points[i].y > vBound(3, 0)*0.25 || cAuxCloud->points[i].z > vBound(5, 0)*0.25)
			{
				cAuxCloudFront->push_back(cAuxCloud->points[i]);
			}
		}
		//clockEnd = clock();
		//printf("Select Pixel %d ms\n", clockEnd - clockBegin);
		nScore = computeSymmetryScore(cAuxCloudFront, cPlane);  //计算相对于平面的对称分数，分数越小越好
		if (nScore < nBestScore)
		{
			nBestScore = nScore;
			nBestTheta = theta;
			pcl::copyPointCloud(*cAuxCloud, *cOutput);
		}

	}

	return nBestTheta;
}


////////////////////////////////////////////////////////////////////////////
// 函数：searchBestRotationAngle
// 描述：寻找最佳的旋转角度，使旋转后点云在某个平面的对称性最佳
// 输入：cInput(输入点云)，axis(旋转轴），cPlane（对称平面系数）
// 输出：cOutput(输出点云)
// 返回：nBestTheta(最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::searchBestRotationAngleForSymmetry_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval)
{
	float nBestScore = 1000;
	float nScore;
	float nBestTheta = 2 * M_PI;
	Eigen::Matrix<float, 6, 1> vBound;
	//time_t clockBegin, clockEnd;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudFront(new pcl::PointCloud<pcl::PointXYZ>);

	for (float theta = theta_base - theta_width; theta < theta_base + theta_width; theta += theta_interval)
	{

		rotateAndTranslate(cInput, cAuxCloud, theta, axis, Eigen::Vector3f(0, 0, 0));  //绕轴旋转
		vBound = getBoundBoxSize(cAuxCloud);
		cAuxCloudFront->clear();
		/*if (axis == Eigen::Vector3f(0, 0, 1)) //提取人头Y轴前1/4处的点
		{
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
		if (cAuxCloud->points[i].y > vBound(3, 0)*0.25)
		{
		cAuxCloudFront->push_back(cAuxCloud->points[i]);
		}
		}
		}
		else  //提取人头Z轴前1/4处的点
		{
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
		if (cAuxCloud->points[i].z > vBound(5, 0)*0.25)
		{
		cAuxCloudFront->push_back(cAuxCloud->points[i]);
		}
		}
		}*/
		//clockBegin = clock();
		for (int i = 0; i < cAuxCloud->points.size(); i++)
		{
			if (cAuxCloud->points[i].y > vBound(3, 0)*0.25 || cAuxCloud->points[i].z > vBound(5, 0)*0.25)
			{
				cAuxCloudFront->push_back(cAuxCloud->points[i]);
			}
		}
		//clockEnd = clock();
		//printf("Select Pixel %d ms\n", clockEnd - clockBegin);
		nScore = computeSymmetryScore(cAuxCloudFront, cPlane);  //计算相对于平面的对称分数，分数越小越好
		if (nScore < nBestScore)
		{
			nBestScore = nScore;
			nBestTheta = theta;
			pcl::copyPointCloud(*cAuxCloud, *cOutput);
		}

	}

	return nBestTheta;
}


////////////////////////////////////////////////////////////////////////////
// 函数：searchBestYZRotationAngleForSymmetry
// 描述：联合寻找绕Y轴和Z轴的旋转角度，使旋转后点云在某个平面的对称性最佳
// 输入：cInput(输入点云)，axis(旋转轴），cPlane（对称平面系数）
// 输出：cOutput(输出点云)
// 返回：nBestTheta(Y、Z的最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
pair<float, float> Head::searchBestYZRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput)
{
	float nBestScore = 1000;
	float nScore;
	pair<float, float> nBestTheta(2 * M_PI, 2 * M_PI);
	Eigen::Matrix<float, 6, 1> vBound;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudTemp(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudSelect(new pcl::PointCloud<pcl::PointXYZ>);

	for (float thetaY = -M_PI / 9; thetaY < M_PI / 9; thetaY += M_PI / 180)
	{
		for (float thetaZ = -M_PI / 9; thetaZ < M_PI / 9; thetaZ += M_PI / 180)
		{
			rotateAndTranslate(cInput, cAuxCloudTemp, thetaY, Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 0, 0));  //绕Y轴旋转
			rotateAndTranslate(cAuxCloudTemp, cAuxCloud, thetaZ, Eigen::Vector3f(0, 0, 1), Eigen::Vector3f(0, 0, 0));  //绕Z轴旋转

			vBound = getBoundBoxSize(cAuxCloud);
			cAuxCloudSelect->clear();
			for (int i = 0; i < cAuxCloud->points.size(); i++) //提取人头Y轴前1/4处的点，且人头Z轴前1/4处的点
			{
				if (cAuxCloud->points[i].y > vBound(3, 0)*0.25 || cAuxCloud->points[i].z > vBound(5, 0)*0.25)
				{
					cAuxCloudSelect->push_back(cAuxCloud->points[i]);
				}
			}

			nScore = computeSymmetryScore(cAuxCloudSelect, cPlane);  //计算相对于平面的对称分数，分数越小越好
			if (nScore < nBestScore)
			{
				nBestScore = nScore;
				nBestTheta = make_pair(thetaY, thetaZ);
				pcl::copyPointCloud(*cAuxCloud, *cOutput);
			}
		}
	}

	return nBestTheta;
}

////////////////////////////////////////////////////////////////////////////
// 函数：searchBestRotationAngleForSmallestAreaOnPlaneXZ
// 描述：寻找最佳的旋转角度，使旋转后点云投影在XZ平面的面积（体素点的个数）最小
// 输入：cInput(输入点云)，axis(旋转轴），nScale（体素的尺度）
// 输出：cOutput(输出点云)
// 返回：nBestTheta(最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::searchBestRotationAngleForSmallestAreaOnPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput)
{
	int nMinArea = INT_MAX;
	int nArea;
	float nBestTheta = 2 * M_PI;
	//clock_t  clockBegin, clockEnd;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudTemp(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);

	for (float theta = -M_PI / 9; theta < M_PI / 9; theta += M_PI / 180)
	{
		//clockBegin = clock();
		rotateAndTranslate(cInput, cAuxCloudTemp, theta, axis, Eigen::Vector3f(0, 0, 0));  //绕轴旋转
																						   //clockEnd = clock();
																						   //printf("%d ms", clockEnd - clockBegin);
		projectToPlane(cAuxCloudTemp, cAuxCloud, Eigen::Vector4f(0, 1, 0, 0));  //投影到XZ平面

		nArea = computerVexelNumOfPlaneXZ(cAuxCloud, nScale);  //计算面积

		if (nArea < nMinArea)
		{
			nMinArea = nArea;
			nBestTheta = theta;
			pcl::copyPointCloud(*cAuxCloudTemp, *cOutput);
		}

	}

	return nBestTheta;
}

////////////////////////////////////////////////////////////////////////////
// 函数：searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2
// 描述：寻找最佳的旋转角度，使旋转后点云投影在XZ平面的面积（体素点的个数）最小
// 输入：cInput(输入点云)，axis(旋转轴），nScale（体素的尺度）
// 输出：cOutput(输出点云) 
// 返回：nBestTheta(最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval)
{
	int nMinArea = INT_MAX;
	int nArea;
	float nBestTheta = 2 * M_PI;
	//clock_t  clockBegin, clockEnd;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudTemp(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);

	for (float theta = theta_base - theta_width; theta < theta_base + theta_width; theta += theta_interval)
	{
		//clockBegin = clock();
		rotateAndTranslate(cInput, cAuxCloudTemp, theta, axis, Eigen::Vector3f(0, 0, 0));  //绕轴旋转
																						   //clockEnd = clock();
																						   //printf("%d ms", clockEnd - clockBegin);
		projectToPlane(cAuxCloudTemp, cAuxCloud, Eigen::Vector4f(0, 1, 0, 0));  //投影到XZ平面

		nArea = computerVexelNumOfPlaneXZ(cAuxCloud, nScale);  //计算面积

		if (nArea < nMinArea)
		{
			nMinArea = nArea;
			nBestTheta = theta;
			pcl::copyPointCloud(*cAuxCloudTemp, *cOutput);
		}

	}

	return nBestTheta;
}

////////////////////////////////////////////////////////////////////////////
// 函数：searchBestXYZRotationAngleForUpRight
// 描述：联合寻找绕X轴、Y轴和Z轴的旋转角度，使旋转后点云投影XZ平面后的面积最小，且根据YZ平面对称
// 输入：cInput(输入点云)
// 输出：cOutput(输出点云)
// 返回：nBestTheta(X、Y、Z的最佳的角度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
std::tuple<float, float, float> Head::searchBestXYZRotationAngleForUpRight(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale)
{
	float nBestResult = INT_MAX;
	float nResult;
	float nScore;
	int nArea;
	std::tuple<float, float, float> nBestTheta(2 * M_PI, 2 * M_PI, 2 * M_PI);
	Eigen::Matrix<float, 6, 1> vBound;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudTemp1(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudTemp2(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloud(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudSelect(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cAuxCloudPlane(new pcl::PointCloud<pcl::PointXYZ>);

	for (float thetaX = -M_PI / 9; thetaX < M_PI / 9; thetaX += M_PI / 180)
	{
		for (float thetaZ = -M_PI / 9; thetaZ < M_PI / 9; thetaZ += M_PI / 180)
		{
			for (float thetaY = -M_PI / 9; thetaY < M_PI / 9; thetaY += M_PI / 180)
			{
				rotateAndTranslate(cInput, cAuxCloudTemp1, thetaY, Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 0, 0));  //绕Y轴旋转
				rotateAndTranslate(cAuxCloudTemp1, cAuxCloudTemp2, thetaZ, Eigen::Vector3f(0, 0, 1), Eigen::Vector3f(0, 0, 0));  //绕Z轴旋转
				rotateAndTranslate(cAuxCloudTemp2, cAuxCloud, thetaX, Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(0, 0, 0));  //绕X轴旋转

				vBound = getBoundBoxSize(cAuxCloud);
				cAuxCloudSelect->clear();
				for (int i = 0; i < cAuxCloud->points.size(); i++) //提取人头Y轴前1/4处的点，且人头Z轴前1/4处的点
				{
					if (cAuxCloud->points[i].y > vBound(3, 0)*0.25 || cAuxCloud->points[i].z > vBound(5, 0)*0.25)
					{
						cAuxCloudSelect->push_back(cAuxCloud->points[i]);
					}
				}
				nScore = computeSymmetryScore(cAuxCloudSelect, Eigen::Vector4f(1, 0, 0, 0));  //计算相对于YZ平面的对称分数，分数越小越好

				projectToPlane(cAuxCloud, cAuxCloudPlane, Eigen::Vector4f(0, 1, 0, 0));  //投影到XZ平面
				nArea = computerVexelNumOfPlaneXZ(cAuxCloudPlane, nScale);  //计算面积，面积越小越好

				nResult = nArea * nScore;    //nResult = nArea + nScore * 500;
				if (nResult < nBestResult)
				{
					nBestResult = nResult;
					nBestTheta = std::tuple<float, float, float>(thetaX, thetaY, thetaZ);
					pcl::copyPointCloud(*cAuxCloud, *cOutput);
				}
			}
		}
	}

	return nBestTheta;
}

////////////////////////////////////////////////////////////////////////////
// 函数：computeCloudResolution
// 描述：计算点云密度（相邻点的平均间距）
// 输入：cInput(输入点云)
// 输出：Null
// 返回：res(点云密度)
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
float Head::computeCloudResolution(pcl::PointCloud<pcl::PointXYZ>::Ptr &cInput, int k)
{
	double res = 0.0;
	int n_points = 0;
	pcl::KdTreeFLANN<pcl::PointXYZ> tree;//利用Kd树来遍历
	tree.setInputCloud(cInput);

	for (int i = 0; i < cInput->size(); ++i)
	{
		if (!pcl_isfinite((*cInput)[i].x))
		{
			continue;
		}

		std::vector<int> indices(k);//指数
		std::vector<float> sqr_distances(k);//平方距离

		if (tree.nearestKSearch(i, k, indices, sqr_distances) == k)
		{
			for (int i = 1; i < k; i++)
			{
				res += sqrt(sqr_distances[i]);
				++n_points;
			}
		}
	}
	if (n_points != 0)
	{
		res /= n_points;
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////
// 函数：rasterize
// 描述：点云三维栅格化
// 输入：cInput(输入点云)
// 输出：cOutput(输出栅格化点云)
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::rasterize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, const float d)
{
	cOutput->clear();

	pcl::PointXYZ minPt, maxPt;
	pcl::getMinMax3D(*cInput, minPt, maxPt);//获取原始点云x,y,z的最值
	int nx = ceil((maxPt.x - minPt.x) / d);
	int ny = ceil((maxPt.y - minPt.y) / d);
	int nz = ceil((maxPt.z - minPt.z) / d);
	int i, j, k;
	int x, y, z;
	vector<bool> temp_z(nz, false);
	vector<vector<bool>> temp_y(ny, temp_z);
	vector<vector<vector<bool>>> cGrid(nx, temp_y);

	for (i = 0; i < cInput->points.size(); i++)
	{
		x = floor((cInput->points.at(i).x - minPt.x) / d);
		y = floor((cInput->points.at(i).y - minPt.y) / d);
		z = floor((cInput->points.at(i).z - minPt.z) / d);
		cGrid[x][y][z] = 1;
	}

	pcl::PointXYZ pSelect;
	for (i = 0; i < nx; i++)
	{
		for (j = 0; j < ny; j++)
		{
			for (k = 0; k < nz; k++)
			{
				if (cGrid[i][j][k] == 1)
				{
					pSelect.x = minPt.x + i * d;
					pSelect.y = minPt.y + j * d;
					pSelect.z = minPt.z + k * d;
					cOutput->points.push_back(pSelect);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// 函数：extractGrid
// 描述：抽取指定层栅格
// 输入：cInput（输入点云栅格），coefficients（指定切面），d（栅格精度）
// 输出：cOutput（输出点云指定栅格层）
// 返回：Null
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void Head::extractGrid(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, const float d)
{
	cOutput->clear();

	pcl::PointXYZ pSelect;
	int i;
	if (coefficients[0] != 0 && coefficients[1] == 0 && coefficients[2] == 0)
	{
		int x = -coefficients[3] / coefficients[0];
		for (i = 0; i < cInput->points.size(); i++)
		{
			if (fabs(cInput->points.at(i).x - x) < (d / 2))
			{
				pSelect.x = x;
				pSelect.y = cInput->points.at(i).y;
				pSelect.z = cInput->points.at(i).z;
				cOutput->points.push_back(pSelect);
			}
		}
	}

	if (coefficients[0] == 0 && coefficients[1] != 0 && coefficients[2] == 0)
	{
		int y = -coefficients[3] / coefficients[1];
		for (i = 0; i < cInput->points.size(); i++)
		{
			if (fabs(cInput->points.at(i).y - y) < (d / 2))
			{
				pSelect.x = cInput->points.at(i).x;
				pSelect.y = y;
				pSelect.z = cInput->points.at(i).z;
				cOutput->points.push_back(pSelect);
			}
		}
	}

	if (coefficients[0] == 0 && coefficients[1] == 0 && coefficients[2] != 0)
	{
		int z = -coefficients[3] / coefficients[2];
		for (i = 0; i < cInput->points.size(); i++)
		{
			if (fabs(cInput->points.at(i).z - z) < (d / 2))
			{
				pSelect.x = cInput->points.at(i).x;
				pSelect.y = cInput->points.at(i).y;
				pSelect.z = z;
				cOutput->points.push_back(pSelect);
			}
		}
	}
}





