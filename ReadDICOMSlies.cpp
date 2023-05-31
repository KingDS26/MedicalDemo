//#include <vtkSmartPointer.h>
//#include <vtkDICOMImageReader.h>
//#include <vtkRenderer.h>
//#include <vtkRenderWindow.h>
//#include <vtkRenderWindowInteractor.h>
//#include <vtkNamedColors.h>
//#include <vtkImageActor.h>
//
//int main() {
//    vtkNew<vtkNamedColors> colors;
//
//    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
//    reader->SetDirectoryName("D:\\DicomDisplay\\vhf");
//    reader->Update();
//
//    vtkNew<vtkImageActor> imageActor;
//    imageActor->SetInputData(reader->GetOutput());
//
//    vtkNew<vtkRenderer> renderer;
//    vtkNew<vtkRenderWindow> renderWindow;
//    renderWindow->AddRenderer(renderer);
//    vtkNew<vtkRenderWindowInteractor> interactor;
//    interactor->SetRenderWindow(renderWindow);
//
//    renderer->AddActor(imageActor);
//    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
//
//    renderer->ResetCamera();
//    renderWindow->SetWindowName("ReadDICOMS");
//    renderWindow->Render();
//
//    interactor->Initialize();
//    interactor->Start();
//
//    return EXIT_SUCCESS;
//}