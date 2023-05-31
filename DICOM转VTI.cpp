#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkSmartPointer.h>

int main03(int argc, char* argv[])
{
    // Verify input arguments
    /*if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " InputDir OutputFile.vti" << std::endl;
        return EXIT_FAILURE;
    }*/

    std::string inputDir = "./vhf/vhf.1502.dcm";
    std::string outputFilename = "./vhf_vti/vhf.1502.vti";

    vtkNew<vtkDICOMImageReader> reader;
    reader->SetDirectoryName(inputDir.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();

    vtkNew<vtkXMLImageDataWriter> writer;
    writer->SetFileName(outputFilename.c_str());
    writer->SetInputData(imageData);
    writer->Write();

    return EXIT_SUCCESS;
}