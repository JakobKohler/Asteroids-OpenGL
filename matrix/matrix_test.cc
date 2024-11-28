#include "matrix.h"
#include "gtest/gtest.h"

namespace {
	
TEST(MATRIX, ListInitialization2df) {
  SquareMatrix2df matrix = { Vector2df{1.0, 0.0},
                             Vector2df{0.0, 1.0} };
  
  EXPECT_NEAR(1.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR(0.0, matrix.at(1,0), 0.00001);
  EXPECT_NEAR(1.0, matrix.at(1,1), 0.00001);
}

TEST(MATRIX, ListInitialization3df) {
  SquareMatrix3df matrix = { Vector3df{1.0, 0.0, 0.0},
                             Vector3df{0.0, 1.0, 0.0},
                             Vector3df{0.0, 0.0, 1.0} };
                               
  EXPECT_NEAR(1.0, matrix[0][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[0][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][0], 0.00001);
  EXPECT_NEAR(1.0, matrix[1][1], 0.00001);
  EXPECT_NEAR(0.0, matrix[1][2], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][0], 0.00001);
  EXPECT_NEAR(0.0, matrix[2][1], 0.00001);
  EXPECT_NEAR(1.0, matrix[2][2], 0.00001);
}

TEST(MATRIX, ListInitialization4df) {
  SquareMatrix4df matrix = { {1.0, 5.0, 9.0, 13.0},
                             {2.0, 6.0, 10.0, 14.0},
                             {3.0, 7.0, 11.0, 15.0},
                             {4.0, 8.0, 12.0, 16.0} };
  
  float v = 1.0f;
  for (size_t row = 0; row < 4; row++) {
    for (size_t column = 0;  column < 4; column++) {    
      EXPECT_NEAR(v++, matrix.at(row, column), 0.00001);
    }
  }
}



TEST(MATRIX, ProductWithVector3df) {
  SquareMatrix3df matrix = { {1.0, 0.0, 1.0},
                             {0.0, 1.0, 1.0},
                             {0.0, 0.0, 1.0} };
  Vector3df vector = {-6.0, 3.0,  1.0};
  Vector3df product = matrix * vector;  
  
  EXPECT_NEAR(-6.0, product[0], 0.00001);
  EXPECT_NEAR( 3.0, product[1], 0.00001);
  EXPECT_NEAR(-2.0, product[2], 0.00001);
}



TEST(MATRIX, ProductWithMatrix2df) {
  SquareMatrix2df matrix1 = { {1.0, 2.0},
                              {-1.0, 1.5} };
  SquareMatrix2df matrix2 = { {2.0, -1.0},
                              {1.0, 0.0} };
  SquareMatrix2df matrix = matrix1 * matrix2;
  
  EXPECT_NEAR( 3.0, matrix.at(0,0), 0.00001);
  EXPECT_NEAR( 1.0, matrix.at(0,1), 0.00001);
  EXPECT_NEAR( 2.5, matrix.at(1,0), 0.00001);
  EXPECT_NEAR( 2.0, matrix.at(1,1), 0.00001);
}
 // MY TESTS
  TEST(MATRIX, IndexOperators) {
  SquareMatrix2df matrix1 = { {1.22, 2.0},
                              {-1.0, 1.5} };

  float upperLeft = matrix1[0][0];
  float lowerRight = matrix1[1][1];

  EXPECT_NEAR( 1.22, upperLeft, 0.00001);
  EXPECT_NEAR( 1.5, lowerRight, 0.00001);
}

  TEST(MATRIX, IndexOperatorsPointer) {
  SquareMatrix2df matrix1 = { {1.22, 2.0},
                              {-1.0, 1.5} };

  Vector2df* firstVector = &matrix1[0];
  auto secondVector = matrix1[1];

  EXPECT_NEAR( 1.22, (*firstVector)[0], 0.00001);
  EXPECT_NEAR( -1.0, secondVector[0], 0.00001);
}

  TEST(MATRIX, MatrixWithVector4d) {
  SquareMatrix4df matrix = { {1.0, 2.0, 3.0, 4.0},
                            {1.0, 2.0, 3.0, 4.0},
                            {1.0, 2.0, 3.0, 4.0},
                              {1.0, 2.0, 3.0, 4.0},
                            };
  SquareMatrix4df matrix2 = { {1.0, 2.0, 3.0, 4.0},
                            {1.0, 2.0, 3.0, 4.0},
                            {1.0, 2.0, 3.0, 4.0},
                              {1.0, 2.0, 3.0, 4.0},
                            };
  SquareMatrix4df res = matrix * matrix2;

  EXPECT_NEAR( 10.0, res.at(0,0), 0.00001);
  EXPECT_NEAR( 20.0, res.at(1,0), 0.00001);
  EXPECT_NEAR( 30.0, res.at(2,0), 0.00001);
  EXPECT_NEAR( 40.0, res.at(3,0), 0.00001);

}

  TEST(MATRIX, MatrixWithVec4d) {
  SquareMatrix4df matrix = { {2, 4, 8, 16},
                            {4, 8, 16, 32},
                            {8, 16, 32, 64},
                              {16, 32, 64, 128},
                            };
  Vector4df vec = { 2,3,4,5 };
  Vector4df res = matrix * vec;

  EXPECT_NEAR( 128.0, res[0], 0.00001);
  EXPECT_NEAR( 256.0, res[1], 0.00001);
  EXPECT_NEAR( 512.0, res[2], 0.00001);
  EXPECT_NEAR( 1024.0, res[3], 0.00001);

}



}
