/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See LICENSE.md for further details
  ----------------------*/

#include "GateArrayRepeater.hh"
#include "GateArrayRepeaterMessenger.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4UnitsTable.hh"

#include "GateTools.hh"
#include "G4Point3D.hh"
#include <cmath>
#include <fstream>

//--------------------------------------------------------------------------------------------
GateArrayRepeater::GateArrayRepeater(GateVVolume *itsObjectInserter,
                                     const G4String &itsName,
                                     G4int itsRepeatNumberX,
                                     G4int itsRepeatNumberY,
                                     G4int itsRepeatNumberZ,
                                     const G4ThreeVector &itsRepeatVector,
                                     G4bool itsFlagAutoCenter)
    : GateVGlobalPlacement(itsObjectInserter, itsName),
      m_repeatVector(itsRepeatVector),
      m_flagAutoCenter(itsFlagAutoCenter),
      m_Messenger(0)
{
  m_repeatNumber[0] = itsRepeatNumberX;
  m_repeatNumber[1] = itsRepeatNumberY;
  m_repeatNumber[2] = itsRepeatNumberZ;

  m_Messenger = new GateArrayRepeaterMessenger(this);
}
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
GateArrayRepeater::~GateArrayRepeater()
{
  delete m_Messenger;
}
//--------------------------------------------------------------------------------------------
/*
  void GateArrayRepeater::PushMyPlacements(const G4RotationMatrix& currentRotationMatrix,
  const G4ThreeVector& currentPosition,
  G4double aTime)
*/
//--------------------------------------------------------------------------------------------
void GateArrayRepeater::PushMyPlacements(const G4RotationMatrix &currentRotationMatrix,
                                         const G4ThreeVector &currentPosition,
                                         G4double)
{
  const double pi = 3.141592653589793238462643383279502884197;
  G4double dx = m_repeatVector[0],
           dy = m_repeatVector[1],
           dz = m_repeatVector[2];

  G4int nx = m_repeatNumber[0],
        ny = m_repeatNumber[1],
        nz = m_repeatNumber[2];

  G4ThreeVector firstCopyOffset(0., 0., 0.);

  if (GetAutoCenterFlag())
    firstCopyOffset = .5 * G4ThreeVector((1 - nx) * dx, (1 - ny) * dy, (1 - nz) * dz);

  G4double x_0 = currentPosition[0] + firstCopyOffset[0],
           y_0 = currentPosition[1] + firstCopyOffset[1],
           z_0 = currentPosition[2] + firstCopyOffset[2];

  // for (G4int k = 0; k < nz; k++)
  //   for (G4int j = 0; j < ny; j++)
  //     for (G4int i = 0; i < nx; i++)
  //     {

  //       G4double x = x_0 + i * dx,
  //                y = y_0 + j * dy,
  //                z = z_0 + k * dz;

  //       G4ThreeVector newPosition = G4ThreeVector(x, y, z);

  //       // fouse coordinates
  //       G4ThreeVector targetPoint(-350, 0, 0);
  //       // 计算当前方向：从当前点指向目标点的方向
  //       G4ThreeVector currentDirection = targetPoint - newPosition;
  //       G4cout << "newPosition   : " << newPosition << "\n";

  //       // 归一化当前方向向量
  //       currentDirection = currentDirection.unit();

  //       // 目标方向，假设我们希望目标方向指向 (15, 0, 0) 并归一化
  //       G4ThreeVector targetDirection(1, 0, 0); // 假设目标是 x 轴方向
  //       targetDirection = targetDirection.unit();

  //       // 计算旋转轴（叉积）
  //       G4ThreeVector axis = currentDirection.cross(targetDirection);
  //       axis = axis.unit(); // 归一化旋转轴

  //       // 计算旋转角度（点积）
  //       G4double dotProduct = currentDirection.dot(targetDirection);
  //       G4double angle = std::acos(dotProduct); // 计算旋转角度

  //       // 构造旋转矩阵
  //       G4RotationMatrix rotation;
  //       rotation.rotate(angle, axis);

  //       // // 应用当前旋转矩阵
  //       rotation = currentRotationMatrix * rotation;
  //       // // origin point
  //       if (abs(y) < 0.0001 && abs(z) < 0.0001)
  //       {
  //         G4RotationMatrix rotation2;
  //         rotation2.rotate(pi, G4ThreeVector(0., 1., 0.));
  //         rotation = currentRotationMatrix * rotation2;
  //       }

  //       // 保存新的旋转和位置
  //       PushBackPlacement(rotation, newPosition);

  //       // PushBackPlacement(currentRotationMatrix,newPosition);
  //     }

  std::string filename = "./ArrayRepeater_placements.txt";
  std::vector<G4Point3D> points;
  std::ifstream inputFile(filename);
  if (!inputFile.is_open())
  {
    G4cerr << "Error: Unable to open file: " << filename << G4endl;
    return;
  }
  std::string line;
  G4double focus=-350;
  if (std::getline(inputFile, line))
  {
    std::istringstream lineStream(line);
    if (!(lineStream >> focus))
    {
      std::cerr << "Error: Invalid format for focus in file." << std::endl;
      return;
    }
  }

  int pointCount = 0;
  if (std::getline(inputFile, line))
  {
    std::istringstream lineStream(line);
    if (!(lineStream >> pointCount))
    {
      std::cerr << "Error: Invalid format for point count in file." << std::endl;
      return;
    }
  }

  G4cout << "points num:" << pointCount << G4endl;
  if (pointCount != nx * ny * nz)
  {
    G4cerr << "Error: Invalid point count in " << filename << " or in main.mac" << G4endl;
    return;
  }
  for (int p = 0; p < pointCount; p++)
  {
    if (std::getline(inputFile, line))
    {
      std::istringstream lineStream(line);
      G4double x, y, z;
      if (!(lineStream >> x >> y >> z))
      {
        std::cerr << "Error: Invalid format for point " << p << " in file." << std::endl;
        return;
      }
      points.push_back(G4Point3D(x, y, z));
    }
  }
  inputFile.close();

  for (auto &point : points)
  {
    G4double x = point[0];
    // G4double x = 20;
    G4double y = point[1];
    G4double z = point[2]; 
    // fouse coordinates
    G4ThreeVector targetPoint(focus, 0, 0);  
    G4ThreeVector newPosition = G4ThreeVector(x, y, z);
    G4double xSlice = 0;
    G4double ratio = (xSlice - x)/(focus - x);
    newPosition = newPosition  + ratio * (targetPoint - newPosition); 

    // 计算当前方向：从当前点指向目标点的方向
    G4ThreeVector currentDirection = targetPoint - newPosition;
    // G4cout << "newPosition   : " << newPosition << "\n";

    // 归一化当前方向向量
    currentDirection = currentDirection.unit();

    // 目标方向，假设我们希望目标方向指向 (focus, 0, 0) 并归一化
    G4ThreeVector targetDirection(1, 0, 0); // 假设目标是 x 轴方向
    targetDirection = targetDirection.unit();

    // 计算旋转轴（叉积）
    G4ThreeVector axis = currentDirection.cross(targetDirection);
    axis = axis.unit(); // 归一化旋转轴

    // 计算旋转角度（点积）
    G4double dotProduct = currentDirection.dot(targetDirection);
    G4double angle = std::acos(dotProduct); // 计算旋转角度

    // 构造旋转矩阵
    G4RotationMatrix rotation;
    rotation.rotate(angle, axis);

    // // 应用当前旋转矩阵
    rotation = currentRotationMatrix * rotation;
    // // origin point
    if (abs(y) < 0.0001 && abs(z) < 0.0001)
    {
      G4RotationMatrix rotation2;
      rotation2.rotate(pi, G4ThreeVector(0., 1., 0.));
      rotation = currentRotationMatrix * rotation2;
    }

    // 保存新的旋转和位置
    PushBackPlacement(rotation, newPosition);

    // PushBackPlacement(currentRotationMatrix,newPosition);
  }
}
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
void GateArrayRepeater::DescribeMyself(size_t indent)
{
  G4cout << GateTools::Indent(indent) << "Repetition type:         " << "cubicArray" << "\n";
  G4cout << GateTools::Indent(indent) << "Nb of copies along X   : " << m_repeatNumber[0] << "\n";
  G4cout << GateTools::Indent(indent) << "Nb of copies along Y   : " << m_repeatNumber[1] << "\n";
  G4cout << GateTools::Indent(indent) << "Nb of copies along Z   : " << m_repeatNumber[2] << "\n";
  G4cout << GateTools::Indent(indent) << "Repetition step along X: " << G4BestUnit(m_repeatVector[0], "Length") << "\n";
  G4cout << GateTools::Indent(indent) << "Repetition step along Y: " << G4BestUnit(m_repeatVector[1], "Length") << "\n";
  G4cout << GateTools::Indent(indent) << "Repetition step along Z: " << G4BestUnit(m_repeatVector[2], "Length") << "\n";
  G4cout << GateTools::Indent(indent) << "Centering:               " << (GetAutoCenterFlag() ? "Yes" : "No") << "\n";
}
//--------------------------------------------------------------------------------------------
