#include "BonsaiCatalystData.h"
#include "vtkBonsaiPipeline.h"
//
#include <vtkMPI.h>
#include "vtkCPDataDescription.h"
#include "vtkCPInputDataDescription.h"
#include "vtkCPProcessor.h"
#include "vtkCPPythonScriptPipeline.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkCellArray.h"
#include <vtkNew.h>

//----------------------------------------------------------------------------
BonsaiCatalystData::BonsaiCatalystData(const int rank, const int nrank,
    const MPI_Comm &comm) :
    RendererData(rank, nrank, comm)
{
  std::cout << "Creating Bonsai Catalyst Data adaptor" << std::endl;
  this->cxxPipeline = vtkSmartPointer<vtkBonsaiPipeline>::New();
  std::string outFilename = BONSAI_CATALYST_OUTPUT_DIR "/bonsai_catalyst_temp.vtk";

  isTimeDataSet = 0;

  if (!coProcessor) {
    coProcessor = vtkSmartPointer<vtkCPProcessor>::New();
    vtkMPICommunicatorOpaqueComm vcomm(const_cast<MPI_Comm*>(&comm));
    coProcessor->Initialize(vcomm);

#ifdef BONSAI_CATALYST_PYTHON
    std::string cPythonFileName = BONSAI_CATALYST_PYTHON_SCRIPT;
    std::cout << "Creating python pipeline " << std::endl;
    vtkCPPythonScriptPipeline* pyPipeline = vtkCPPythonScriptPipeline::New();
    std::cout << "Initializing python pipeline " << std::endl;
    pyPipeline->Initialize(cPythonFileName.c_str());
    std::cout << "Python pipeline initialized " << std::endl;
    coProcessor->AddPipeline(pyPipeline);
#else
    std::cout << "Creating cxx pipeline " << std::endl;
    cxxPipeline = vtkSmartPointer<vtkBonsaiPipeline>::New();
    std::cout << "Initializing cxx pipeline " << std::endl;
    cxxPipeline->Initialize(1, outFilename);
    coProcessor->AddPipeline(cxxPipeline);
#endif
  }
  if (!coProcessorData) {
    coProcessorData = vtkSmartPointer<vtkCPDataDescription>::New();
    coProcessorData->AddInput("input");
  }

  particles = vtkSmartPointer<vtkPolyData>::New();
}

//----------------------------------------------------------------------------
BonsaiCatalystData::~BonsaiCatalystData() {
}
//----------------------------------------------------------------------------
void BonsaiCatalystData::coProcess(double time, unsigned int timeStep)
{
  std::cout << "Calling coProcess" << std::endl;

  if (!coProcessorData) {
    coProcessorData = vtkSmartPointer<vtkCPDataDescription>::New();
    coProcessorData->AddInput("input");
    coProcessorData->SetTimeData(time, timeStep);
  }

  if (coProcessor->RequestDataDescription(coProcessorData.GetPointer()) != 0) {
    // for each field array generate a vtk equivalent array
    const char *names[] = { "MASS", "VEL", "RHO", "H" };
    std::vector<vtkSmartPointer<vtkFloatArray> > fieldArrays;
    for (int p = 0; p < NPROP; p++) {
      fieldArrays.push_back(vtkSmartPointer<vtkFloatArray>::New());
      fieldArrays[p]->SetNumberOfTuples(data.size());
      fieldArrays[p]->SetName(names[p]);
    }

    // create the points information,
    // copy from particle_t struct into vtk array
    // in next version, use vtk array adaptor for zero copy
    // but first get working using a copy.
    vtkNew<vtkFloatArray> pointArray;
    pointArray->SetNumberOfComponents(3);
    pointArray->SetNumberOfTuples(data.size());
    float *pointarray = pointArray->GetPointer(0);
    for (int i = 0; i < data.size(); i++) {
      pointarray[i * 3 + 0] = data[i].posx;
      pointarray[i * 3 + 1] = data[i].posy;
      pointarray[i * 3 + 2] = data[i].posz;
      for (int p = 0; p < NPROP; p++) {
        fieldArrays[p]->SetValue(i, data[i].attribute[p]);
      }
    }
    for (int p = 0; p < NPROP; p++) {
      particles->GetPointData()->AddArray(fieldArrays[p]);
    }

    //
    // generate cells
    //
    if (1/*this->GenerateVertexCells*/) {
      vtkIdType Nt = data.size();
      vtkSmartPointer<vtkCellArray> vertices =
          vtkSmartPointer<vtkCellArray>::New();
      vtkIdType *cells = vertices->WritePointer(Nt, 2 * Nt);
      for (vtkIdType i = 0; i < Nt; ++i) {
        cells[2 * i] = 1;
        cells[2 * i + 1] = i;
      }
      particles->SetVerts(vertices);
    }

    vtkNew<vtkPoints> points;
    points->SetData(pointArray.GetPointer());
    particles->SetPoints(points.GetPointer());

    coProcessorData->GetInputDescriptionByName("input")->SetGrid(particles);

    coProcessor->CoProcess(coProcessorData.GetPointer());
  }
}
