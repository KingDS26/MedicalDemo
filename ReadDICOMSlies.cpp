#include<vtkSmartPointer.h>
#include<vtkImageViewer2.h>
#include<vtkDICOMImageReader.h>
#include<vtkRenderWindow.h>
#include<vtkRenderWindowInteractor.h>
#include<vtkRenderer.h>
#include<vtkTextProperty.h>
#include<vtkTextMapper.h>
#include<vtkActor2D.h>
#include<vtkInteractorStyleImage.h>
#include<vtkObjectFactory.h>
#include<iostream>
#include<string.h>
#include<vtkAutoInit.h>
#include<sstream>


VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)

using namespace std;

class StatusMessage {
public:
	static std::string Format(int slice, int maxSlice)
	{
		std::stringstream tmp;
		tmp << "Slice Number  " << slice + 1 << "/" << maxSlice + 1;

		return tmp.str();
	}

};

// Define own interation style;
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
	static myVtkInteractorStyleImage* New();
	vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
	vtkImageViewer2* _ImageViewer;
	vtkTextMapper* _StatusMapper;
	int _Slice;
	int _MinSlice;
	int _MaxSlice;

public:
	void SetImageViewer(vtkImageViewer2* imageViwer)
	{
		_ImageViewer = imageViwer;
		_MinSlice = imageViwer->GetSliceMin();
		_MaxSlice = imageViwer->GetSliceMax();
		_Slice = _MinSlice;

		cout << "Slice : Min = " << _MinSlice << ", Max = " << _MaxSlice << endl;

	}

	void SetStatusMapper(vtkTextMapper* statusMapper)
	{
		_StatusMapper = statusMapper;
	}

protected:
	void MoveSliceForward()
	{
		if (_Slice < _MaxSlice)
		{
			_Slice += 1;
			cout << " MoveSliceForward::Slice = " << _Slice << endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();

		}
	}

	void MoveSliceBackward()
	{
		if (_Slice > _MinSlice)
		{
			_Slice -= 1;
			cout << "MoveSliceBackward::Slice = " << _Slice << endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();

		}
	}

	virtual void OnKeyDown()
	{
		string key = this->GetInteractor()->GetKeySym();
		if (key.compare("Up") == 0)
		{
			MoveSliceForward();
		}
		else if (key.compare("Down") == 0)
		{
			MoveSliceBackward();
		}
		vtkInteractorStyleImage::OnKeyDown();

	}

	virtual void OnMouseWheelForward()
	{
		MoveSliceForward();
	}

	virtual void OnMouseWheelBackward()
	{
		if (_Slice > _MinSlice)
		{
			MoveSliceBackward();
		}
	}
};


vtkStandardNewMacro(myVtkInteractorStyleImage);



int main()
{
	using namespace std;

	string OpenPath = "./vhf";

	vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetDirectoryName(OpenPath.c_str());
	reader->Update();


	vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
	imageViewer->SetInputConnection(reader->GetOutputPort());

	//Slice Status
	vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
	sliceTextProp->SetFontFamilyToCourier();
	sliceTextProp->SetFontSize(20);
	sliceTextProp->SetVerticalJustificationToBottom();
	sliceTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
	sliceTextMapper->SetInput(msg.c_str());
	cout << "msg" << msg.c_str() << endl;

	sliceTextMapper->SetTextProperty(sliceTextProp);

	vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
	sliceTextActor->SetMapper(sliceTextMapper);
	sliceTextActor->SetPosition(15, 10);


	//usage hint message;
	vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
	usageTextProp->SetFontFamilyToCourier();
	usageTextProp->SetFontSize(14);
	usageTextProp->SetVerticalJustificationToTop();
	usageTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	usageTextMapper->SetInput("Mouse wheel or up and down keys to switch");
	usageTextMapper->SetTextProperty(usageTextProp);

	vtkSmartPointer<vtkActor2D> usageTextActor = vtkSmartPointer <vtkActor2D>::New();
	usageTextActor->SetMapper(usageTextMapper);
	usageTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);


	vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle = vtkSmartPointer<myVtkInteractorStyleImage>::New();

	myInteractorStyle->SetImageViewer(imageViewer);
	myInteractorStyle->SetStatusMapper(sliceTextMapper);


	vtkSmartPointer<vtkRenderWindowInteractor> renWin = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	imageViewer->SetupInteractor(renWin);
	renWin->SetInteractorStyle(myInteractorStyle);

	imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
	imageViewer->GetRenderer()->AddActor2D(usageTextActor);


	imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	imageViewer->Render();

	renWin->Start();

	return EXIT_SUCCESS;
}
