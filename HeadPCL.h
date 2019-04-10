#pragma once
#pragma warning(disable:4996)//避免版本不兼容或者头文件编译不兼容
#ifndef HEADPCL_H
#define HEADPCL_H  //防止重复包含的宏定义
#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkDICOMImageReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkOutlineFilter.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkContourFilter.h"
#include "vtkVolume16Reader.h"
#include "vtkImageCast.h"
#include "vtkImageReader.h"
#include "vtkBMPReader.h"
#include "vtkMarchingCubes.h"
#include "vtkStripper.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkDecimatePro.h"
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCylinderSource.h>

#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include "stdio.h"
#include <pcl/io/pcd_io.h> 
#include <pcl/io/ply_io.h>
#include <pcl/io/vtk_lib_io.h>
#include <pcl/common/common.h>
#include <pcl/common/transforms.h>
#include <pcl/common/geometry.h>
#include <pcl/point_types.h> 
#include <pcl/point_cloud.h>
#include <pcl/filters/voxel_grid.h> 
#include <pcl/filters/project_inliers.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/passthrough.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/PolygonMesh.h>
#include <pcl/surface/mls.h>
#include <pcl/surface/poisson.h>
#include <boost/thread/thread.hpp>
#include <pcl/search/kdtree.h>
#include <pcl/features/normal_3d_omp.h>
#include <windows.h>

using namespace std;
using namespace pcl;

class Head
{

public:

	//默认构造函数
	Head();

	//默认析构函数
	~Head();

	//对齐点云
	pcl::PointCloud<pcl::PointXYZ>::Ptr alignModel(const string filePath, pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight);

	//点云表面重建
	PolygonMesh restruction(pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight);

	//在点云窗口显示点云模型
	void showModel(PolygonMesh mesh, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer);
	
	//获取点云关键点
	pcl::PointCloud<pcl::PointXYZ>::Ptr getKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, pcl::PointCloud<pcl::PointXYZ>::Ptr cInput);

	//显示点云关键点
	void showKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer);

	//更正关键点
	//pcl::PointCloud<pcl::PointXYZ>::Ptr correctKeyPoints(int order);

	//计算头围
	float computeHeadCircumference(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointXYZ glabellumPt);

	//计算两点三维距离
	float computeDistance3D(pcl::PointXYZ pA, pcl::PointXYZ pB);

	/////共有成员变量
	int keyOrder;
	pcl::PointXYZ current_point;
	pcl::PointCloud<pcl::PointXYZ>::Ptr clicked_points;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight;
	pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr;
	pcl::PointCloud<pcl::PointXYZ>::Ptr correctedPtr;

	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud;
	PolygonMesh mesh;

	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer;
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer_2;
	boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer_3;
	std::string filename;
	std::string filename_keyPoints;

private:

	//计算关键尺寸
	const char* computeKeySize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, int keyOrder);

	//鼠标取点响应
	//void pp_callback(const pcl::visualization::PointPickingEvent& event, void* args);
	//void pp_callback_2(const pcl::visualization::PointPickingEvent& event, void* args);

	//计算两点二维距离
	float computeDistance2D(pcl::PointXYZ pointA, pcl::PointXYZ pointB);

	//计算三点的面积
	float computeTriangleArea(pcl::PointXYZ pA, pcl::PointXYZ pB, pcl::PointXYZ pC);

	//计算点到面的距离
	float computePointToPlaneDistance(pcl::PointXYZ pA, Eigen::Vector4f cPlane);

	//计算点云对于一个面的对称性
	float computeSymmetryScore(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f coefficients);

	//计算投影面体素的个数，投影面在XZ平面上
	int computerVexelNumOfPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInputPlane, float nScale);

	//计算两向量角度
	float computeAngle(float x1, float y1, float x2, float y2);

	//降采样点云
	void downSample(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//减去点云质心
	void deMean(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//点云尺度变换
	void scaleTransform(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float scale);

	//旋转和平移点云
	void rotateAndTranslate(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta, Eigen::Vector3f axis, Eigen::Vector3f displacement);

	//投影点云到平面
	void projectToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients);

	//点云截面提取
	void sliceToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, float nThreshold);

	//以体素为单位对点云增采样
	void upSamplingInVexel(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//寻找最佳的旋转角度，使旋转后点云在某个平面的对称性最佳
	float searchBestRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//寻找最佳的旋转角度，使旋转后点云在某个平面的对称性最佳（第二个版本，输入搜索范围）
	float searchBestRotationAngleForSymmetry_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval);

	//寻找最佳的旋转角度，使旋转后点云投影在XZ平面的面积（体素点的个数）最小
	float searchBestRotationAngleForSmallestAreaOnPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//寻找最佳的旋转角度，使旋转后点云投影在XZ平面的面积（体素点的个数）最小（第二个版本，输入搜索范围）
	float searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval);

	//联合寻找绕Y轴和Z轴的旋转角度，使旋转后点云在某个平面的对称性最佳
	std::pair<float, float> searchBestYZRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//联合寻找绕X轴、Y轴和Z轴的旋转角度，使旋转后点云投影XZ平面后的面积最小，且根据YZ平面对称
	std::tuple<float, float, float> searchBestXYZRotationAngleForUpRight(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//计算点云平均密度
	float computeCloudResolution(pcl::PointCloud<pcl::PointXYZ>::Ptr &cInput, int k);

	//点云三维栅格化
	void rasterize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, const float d);

	//抽取指定层栅格
	void extractGrid(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, const float d);

	//计算点云边界尺寸坐标：xmin,xmax,ymin,ymax,zmin,zmax
	Eigen::Matrix<float, 6, 1> getBoundBoxSize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput);
};
#endif

