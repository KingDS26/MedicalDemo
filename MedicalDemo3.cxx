#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkImageMapToColors.h>
#include <vtkImageMapper3D.h>
#include <vtkLookupTable.h>
#include <vtkMetaImageReader.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOutlineFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStripper.h>
#include <vtkVersion.h>
#include <array>
#include <vtkDICOMImageReader.h>

// vtkFlyingEdges3D was introduced in VTK >= 8.2
#if VTK_MAJOR_VERSION >= 9 || (VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 2)
#define USE_FLYING_EDGES
#else
#undef USE_FLYING_EDGES
#endif

#ifdef USE_FLYING_EDGES
#include <vtkFlyingEdges3D.h>
#else
#include <vtkMarchingCubes.h>
#endif

int main(int argc, char* argv[])
{
    vtkNew<vtkNamedColors> colors;

    std::array<unsigned char, 4> skinColor{ {250, 184, 160, 255} };  // 240 184 160 255
    colors->SetColor("SkinColor", skinColor.data());
    std::array<unsigned char, 4> bkg{ {51, 77, 102, 255} };  // 51 77 102 255
    colors->SetColor("BkgColor", bkg.data());

    /*
    ������Ⱦ������Ⱦ���ںͽ�������
    ��Ⱦ������Ⱦ�����н��л��ƣ�
    ����������Ⱦ������ʵ�ֻ������ͼ��̵����ݽ�����
    */
    vtkNew<vtkRenderer> aRenderer;
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(aRenderer);  // ����Ⱦ����ӵ���Ⱦ������
    renWin->SetWindowName("MedicalDemo3");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);  // ����Ⱦ������ӵ���������

    // Set a background color for the renderer and set the size of the
    // render window (expressed in pixels).
    aRenderer->SetBackground(colors->GetColor3d("BkgColor").GetData());
    renWin->SetSize(640, 480);

    /*
        ����Ķ�������������ȡһϵ�й�������Ķ�ά��Ƭ��ͼ�񣩵ġ�
        ��Ƭ�ĳߴ�����صļ�඼Ҫ�趨��
        ���ݵ�EndiannessҲ���뱻ָ����
        ��ȡ��ʹ��FilePrefix����Ƭ��������������ļ�������ʽΪFilePrefix.%d��
        (����������� FilePrefix���ļ��ĸ����ƣ�quarter����
    */

    /*
    vtkNew<vtkMetaImageReader> reader;
    reader->SetFileName("D:\\DicomDisplay\\seg-lungs-LUNA16\\1.3.6.1.4.1.14519.5.2.1.6279.6001.111172165674661221381920536987.mhd");
    reader->Update();
    */

    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName("D:\\DicomDisplay\\vhf");
    reader->Update();

    // An isosurface, or contour value of 500 is known to correspond to
    // the skin of the patient.
    // The triangle stripper is used to create triangle
    // strips from the isosurface; these render much faster on may
    // systems.
#ifdef USE_FLYING_EDGES
    vtkNew<vtkFlyingEdges3D> skinExtractor;  // Ƥ��
#else
    vtkNew<vtkMarchingCubes> skinExtractor;
#endif
    skinExtractor->SetInputConnection(reader->GetOutputPort());
    skinExtractor->SetValue(0, -300); 
    skinExtractor->Update();

    vtkNew<vtkStripper> skinStripper;
    skinStripper->SetInputConnection(skinExtractor->GetOutputPort());
    skinStripper->Update();

    vtkNew<vtkPolyDataMapper> skinMapper;
    skinMapper->SetInputConnection(skinStripper->GetOutputPort());
    skinMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> skin;
    skin->SetMapper(skinMapper);
    skin->GetProperty()->SetDiffuseColor(
        colors->GetColor3d("SkinColor").GetData());
    skin->GetProperty()->SetSpecular(0.7);  // ����
    skin->GetProperty()->SetSpecularPower(30);

    // An isosurface, or contour value of 1150 is known to correspond to
    // the bone of the patient.
    // The triangle stripper is used to create triangle
    // strips from the isosurface; these render much faster on may
    // systems.
#ifdef USE_FLYING_EDGES
    vtkNew<vtkFlyingEdges3D> boneExtractor;  // ��ͷ
#else
    vtkNew<vtkMarchingCubes> boneExtractor;
#endif
    boneExtractor->SetInputConnection(reader->GetOutputPort());
    boneExtractor->SetValue(0, 1500);

    vtkNew<vtkStripper> boneStripper;
    boneStripper->SetInputConnection(boneExtractor->GetOutputPort());

    vtkNew<vtkPolyDataMapper> boneMapper;
    boneMapper->SetInputConnection(boneStripper->GetOutputPort());
    boneMapper->ScalarVisibilityOff();

    vtkNew<vtkActor> bone;
    bone->SetMapper(boneMapper);
    bone->GetProperty()->SetDiffuseColor(colors->GetColor3d("Ivory").GetData());

    // An outline provides context around the data.
    // �����
    vtkNew<vtkOutlineFilter> outlineData;
    outlineData->SetInputConnection(reader->GetOutputPort());
    outlineData->Update();

    vtkNew<vtkPolyDataMapper> mapOutline;
    mapOutline->SetInputConnection(outlineData->GetOutputPort());

    vtkNew<vtkActor> outline;
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(colors->GetColor3d("Black").GetData());


    /*
        ���ڣ��������ڴ��������������������ƽ�档ÿ��ƽ��ʹ�ò�ͬ��������ͼ������� ��ͬ����ɫ��
    */

    // Start by creating a black/white lookup table.
    vtkNew<vtkLookupTable> bwLut;  // �ڰײ��ұ�
    bwLut->SetTableRange(-1000, 1500);
    bwLut->SetSaturationRange(1, 1);  // 0 0
    bwLut->SetHueRange(0, 1);         // 0 0
    bwLut->SetValueRange(0, 1);       // 0 1
    bwLut->Build(); // effective built

    // Now create a lookup table that consists of the full hue circle from HSV
    // ����������HSVɫ��Ȧ
    vtkNew<vtkLookupTable> hueLut;  // ɫ�����ұ�
    hueLut->SetTableRange(-1000, 1500);
    hueLut->SetHueRange(0, 1);
    hueLut->SetSaturationRange(1, 1);
    hueLut->SetValueRange(0, 1);   // 1 1 ��ʾ��ɫ
    hueLut->Build(); // effective built

    // Finally, create a lookup table with a single hue but having a range
    // in the saturation of the hue.
    // ��󣬴���һ�����е�һɫ���Ĳ��ұ�����ɫ���ı��Ͷ�����һ����Χ��
    vtkNew<vtkLookupTable> satLut;
    satLut->SetTableRange(-1000, 1500);
    satLut->SetHueRange(0, 1);     //  0.6 0.6
    satLut->SetSaturationRange(1, 1);  // 0  1
    satLut->SetValueRange(0, 1);   // 1 1 ��ʾ��ɫ
    satLut->Build(); // effective built


    /*
        ��������ƽ���еĵ�һ����������vtkImageMapToColorsͨ�����洴������Ӧ���ұ���ӳ�����ݡ�
        �� vtkImageActor �� vtkProp ��һ�����ͣ����Է������һ���ı���ƽ������ʾͼ��
        (ע�⣺�����ͼ��������޷��ŵ�charֵ������vtkImageMapToColors�����ġ�) 
        ��Ҫע����ǣ�ͨ��ָ��DisplayExtent����ˮ�߻����������Χ������ ��vtkImageMapToColorsֻ�������ݵ�һ��Ƭ�ϡ�
    */
    vtkNew<vtkImageMapToColors> sagittalColors;
    sagittalColors->SetInputConnection(reader->GetOutputPort());
    sagittalColors->SetLookupTable(bwLut);
    sagittalColors->Update();

    vtkNew<vtkImageActor> sagittal;
    sagittal->GetMapper()->SetInputConnection(sagittalColors->GetOutputPort());
    sagittal->SetDisplayExtent(256, 256, 0, 512, 0, 256);  // ���� ��ɫ ��λ��
    sagittal->ForceOpaqueOn();

    // Create the second (axial) plane of the three planes. We use the
    // same approach as before except that the extent differs.
    vtkNew<vtkImageMapToColors> axialColors;
    axialColors->SetInputConnection(reader->GetOutputPort());
    axialColors->SetLookupTable(hueLut);
    axialColors->Update();

    vtkNew<vtkImageActor> axial;
    axial->GetMapper()->SetInputConnection(axialColors->GetOutputPort());
    axial->SetDisplayExtent(0, 512, 0, 512, 120, 120);  // ���� ��ɫ ��λ��
    axial->ForceOpaqueOn();

    // Create the third (coronal) plane of the three planes. We use
    // the same approach as before except that the extent differs.
    vtkNew<vtkImageMapToColors> coronalColors;
    coronalColors->SetInputConnection(reader->GetOutputPort());
    coronalColors->SetLookupTable(satLut);
    coronalColors->Update();

    vtkNew<vtkImageActor> coronal;
    coronal->GetMapper()->SetInputConnection(coronalColors->GetOutputPort());
    coronal->SetDisplayExtent(0, 512, 256, 256, 0, 256);  // ���� ��ɫ ��λ��
    coronal->ForceOpaqueOn();

    // It is convenient to create an initial view of the data. The
    // FocalPoint and Position form a vector direction. Later on
    // (ResetCamera() method) this vector is used to position the camera
    // to look at the data in this direction.
    vtkNew<vtkCamera> aCamera;
    aCamera->SetViewUp(0, 0, -1);
    aCamera->SetPosition(0, -1, 0);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    aCamera->Azimuth(30.0);
    aCamera->Elevation(30.0);

    // Actors are added to the renderer.
    aRenderer->AddActor(outline);
    aRenderer->AddActor(sagittal);
    aRenderer->AddActor(axial);
    aRenderer->AddActor(coronal);
    aRenderer->AddActor(skin);
    aRenderer->AddActor(bone);

    // Turn off bone for this example.
    bone->VisibilityOff();

    // Set skin to semi-transparent.
    skin->GetProperty()->SetOpacity(0.3);

    // An initial camera view is created. The Dolly() method moves
    // the camera towards the FocalPoint, thereby enlarging the image.
    aRenderer->SetActiveCamera(aCamera);

    // Calling Render() directly on a vtkRenderer is strictly forbidden.
    // Only calling Render() on the vtkRenderWindow is a valid call.
    renWin->Render();

    aRenderer->ResetCamera();
    aCamera->Dolly(1.5);

    // Note that when camera movement occurs (as it does in the Dolly()
    // method), the clipping planes often need adjusting. Clipping planes
    // consist of two planes: near and far along the view direction. The
    // near plane clips out objects in front of the plane; the far plane
    // clips out objects behind the plane. This way only what is drawn
    // between the planes is actually rendered.
    aRenderer->ResetCameraClippingRange();

    // interact with data
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}
