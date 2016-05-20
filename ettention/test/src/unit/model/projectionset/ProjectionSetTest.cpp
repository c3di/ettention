#include "stdafx.h"
#include "model/projectionset/ProjectionSet.h"
#include "model/projectionset/ProjectionSetIdentity.h"
#include "model/projectionset/ProjectionSetRandom.h"
#include "model/projectionset/ProjectionSetMaxAngle.h"
#include "model/volume/StackComparator.h"
#include "framework/test/TestBase.h"
#include "io/HyperStackIndex.h"

namespace ettention
{
    class ProjectionSetTest : public TestBase
    {
    public:
        ProjectionSet* set;
    };
}

using namespace ettention;

// 
// Back Projection Tests
//
TEST_F(ProjectionSetTest, Unit_Constructor_Identity) 
{
    set = new ProjectionSetIdentity();
    delete set;
}                                                           

TEST_F(ProjectionSetTest, Unit_Positive_Identity)
{
    set = new ProjectionSetIdentity;
    set->setProjectionCount(5, std::set<HyperStackIndex>());
    ASSERT_EQ(set->getProjectionIndex(0), 0);
    ASSERT_EQ(set->getProjectionIndex(1), 1);
    ASSERT_EQ(set->getProjectionIndex(2), 2);
    ASSERT_EQ(set->getProjectionIndex(3), 3);
    ASSERT_EQ(set->getProjectionIndex(4), 4);
    delete set;
}

TEST_F(ProjectionSetTest, Unit_SkipList_Identity)
{
    set = new ProjectionSetIdentity;
    std::set<HyperStackIndex> skipSet;
    const HyperStackIndex first_element(0);
    const HyperStackIndex any_element(2);
    const HyperStackIndex last_element(4);
    skipSet.insert(first_element);
    skipSet.insert(any_element);
    skipSet.insert(last_element);
    const unsigned int any_value = 5;

    set->setProjectionCount(any_value, skipSet);
    ASSERT_EQ(set->getProjectionIndex(0), 1);
    ASSERT_EQ(set->getProjectionIndex(1), 3);
    ASSERT_EQ(set->getNumberOfProjections(), 2);
    delete set;
}

TEST_F(ProjectionSetTest, Unit_Constructor_MaxAngle) 
{
    set = new ProjectionSetMaxAngle;
    delete set;
} 

TEST_F(ProjectionSetTest, Unit_Positive_Random)
{
    set = new ProjectionSetRandom;
    const unsigned int numberOfTestedProjections = 5;
    set->setProjectionCount(numberOfTestedProjections, std::set<HyperStackIndex>());
    std::set<HyperStackIndex> foundNumbers;
    for(int i = 0; i < numberOfTestedProjections; i++)
    {
        foundNumbers.insert(set->getProjectionIndex(i));
    }
    ASSERT_TRUE(foundNumbers.size() == numberOfTestedProjections);
    for(int i = 0; i < numberOfTestedProjections; i++)
    {
        ASSERT_TRUE(foundNumbers.find(HyperStackIndex(i)) != foundNumbers.end());
    }
    delete set;
}
