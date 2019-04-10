#pragma once
#pragma warning(disable:4996)//����汾�����ݻ���ͷ�ļ����벻����
#ifndef HEADPCL_H
#define HEADPCL_H  //��ֹ�ظ������ĺ궨��
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

	//Ĭ�Ϲ��캯��
	Head();

	//Ĭ����������
	~Head();

	//�������
	pcl::PointCloud<pcl::PointXYZ>::Ptr alignModel(const string filePath, pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight);

	//���Ʊ����ؽ�
	PolygonMesh restruction(pcl::PointCloud<pcl::PointXYZ>::Ptr cUpRight);

	//�ڵ��ƴ�����ʾ����ģ��
	void showModel(PolygonMesh mesh, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer);
	
	//��ȡ���ƹؼ���
	pcl::PointCloud<pcl::PointXYZ>::Ptr getKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, pcl::PointCloud<pcl::PointXYZ>::Ptr cInput);

	//��ʾ���ƹؼ���
	void showKeyPoints(pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer);

	//�����ؼ���
	//pcl::PointCloud<pcl::PointXYZ>::Ptr correctKeyPoints(int order);

	//����ͷΧ
	float computeHeadCircumference(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointXYZ glabellumPt);

	//����������ά����
	float computeDistance3D(pcl::PointXYZ pA, pcl::PointXYZ pB);

	/////���г�Ա����
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

	//����ؼ��ߴ�
	const char* computeKeySize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr keyPtr, int keyOrder);

	//���ȡ����Ӧ
	//void pp_callback(const pcl::visualization::PointPickingEvent& event, void* args);
	//void pp_callback_2(const pcl::visualization::PointPickingEvent& event, void* args);

	//���������ά����
	float computeDistance2D(pcl::PointXYZ pointA, pcl::PointXYZ pointB);

	//������������
	float computeTriangleArea(pcl::PointXYZ pA, pcl::PointXYZ pB, pcl::PointXYZ pC);

	//����㵽��ľ���
	float computePointToPlaneDistance(pcl::PointXYZ pA, Eigen::Vector4f cPlane);

	//������ƶ���һ����ĶԳ���
	float computeSymmetryScore(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f coefficients);

	//����ͶӰ�����صĸ�����ͶӰ����XZƽ����
	int computerVexelNumOfPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInputPlane, float nScale);

	//�����������Ƕ�
	float computeAngle(float x1, float y1, float x2, float y2);

	//����������
	void downSample(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//��ȥ��������
	void deMean(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//���Ƴ߶ȱ任
	void scaleTransform(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float scale);

	//��ת��ƽ�Ƶ���
	void rotateAndTranslate(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta, Eigen::Vector3f axis, Eigen::Vector3f displacement);

	//ͶӰ���Ƶ�ƽ��
	void projectToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients);

	//���ƽ�����ȡ
	void sliceToPlane(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, float nThreshold);

	//������Ϊ��λ�Ե���������
	void upSamplingInVexel(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//Ѱ����ѵ���ת�Ƕȣ�ʹ��ת�������ĳ��ƽ��ĶԳ������
	float searchBestRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//Ѱ����ѵ���ת�Ƕȣ�ʹ��ת�������ĳ��ƽ��ĶԳ�����ѣ��ڶ����汾������������Χ��
	float searchBestRotationAngleForSymmetry_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval);

	//Ѱ����ѵ���ת�Ƕȣ�ʹ��ת�����ͶӰ��XZƽ�����������ص�ĸ�������С
	float searchBestRotationAngleForSmallestAreaOnPlaneXZ(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//Ѱ����ѵ���ת�Ƕȣ�ʹ��ת�����ͶӰ��XZƽ�����������ص�ĸ�������С���ڶ����汾������������Χ��
	float searchBestRotationAngleForSmallestAreaOnPlaneXZ_V2(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector3f axis, float nScale, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float theta_base, float theta_width, float theta_interval);

	//����Ѱ����Y���Z�����ת�Ƕȣ�ʹ��ת�������ĳ��ƽ��ĶԳ������
	std::pair<float, float> searchBestYZRotationAngleForSymmetry(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, Eigen::Vector4f cPlane, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput);

	//����Ѱ����X�ᡢY���Z�����ת�Ƕȣ�ʹ��ת�����ͶӰXZƽ���������С���Ҹ���YZƽ��Գ�
	std::tuple<float, float, float> searchBestXYZRotationAngleForUpRight(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, float nScale);

	//�������ƽ���ܶ�
	float computeCloudResolution(pcl::PointCloud<pcl::PointXYZ>::Ptr &cInput, int k);

	//������άդ��
	void rasterize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, const float d);

	//��ȡָ����դ��
	void extractGrid(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput, pcl::PointCloud<pcl::PointXYZ>::Ptr cOutput, Eigen::Vector4f coefficients, const float d);

	//������Ʊ߽�ߴ����꣺xmin,xmax,ymin,ymax,zmin,zmax
	Eigen::Matrix<float, 6, 1> getBoundBoxSize(pcl::PointCloud<pcl::PointXYZ>::Ptr cInput);
};
#endif

