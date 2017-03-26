/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/
/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : TestParser.cpp
* Creation Date : 20-01-2015
* Last Modified : Do 26 Mär 2015 12:58:58 CET
* Created By : Fabian Frie
_._._._._._._._._._._._._._._._._._._._._.*/

#include "TestParser.h"
static const double sDelta = 0.000001;

template <typename T, typename FUNC>
void SimpleTestcases( FUNC GetElement)
{
    const char *xmlList = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyList>\
                1, 2, 3, 4, 5;\
            </MyList>\
        </MyRoot>";
    boost::scoped_ptr<xmlparser::tinyxml2::XmlParserImpl > parser(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlList);
    boost::shared_ptr<xmlparser::XmlParameter> param( parser->GetRoot() );

    std::vector<T> myVec = (*(param.get()).*GetElement)("MyList");
    TS_ASSERT_EQUALS(myVec.size(),5);
    TS_ASSERT_DELTA(myVec.at(0),1.0, sDelta);
    TS_ASSERT_DELTA(myVec.at(1),2.0, sDelta);
    TS_ASSERT_DELTA(myVec.at(2),3.0, sDelta);
    TS_ASSERT_DELTA(myVec.at(3),4.0, sDelta);
    TS_ASSERT_DELTA(myVec.at(4),5.0, sDelta);

    const char *xmlListShortNull = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyList>\
                0;\
            </MyList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlListShortNull);
    param =  parser->GetRoot();
    std::vector<T> myVecShortNull = (*(param.get()).*GetElement)("MyList");

    TS_ASSERT_EQUALS(myVecShortNull.size(),1);
    TS_ASSERT_DELTA(myVecShortNull.at(0),0, sDelta);

    const char *xmlListShort = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyList>\
                5;\
            </MyList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlListShort);
    param =  parser->GetRoot();
    std::vector<T> myVecShort = (*(param.get()).*GetElement)("MyList");

    TS_ASSERT_EQUALS(myVecShort.size(),1);
    TS_ASSERT_DELTA(myVecShort.at(0),5, sDelta);
}

void TestParser::TestGetElementUnsignedIntVecValue()
{
    SimpleTestcases<size_t>( &xmlparser::XmlParameter::GetElementUnsignedIntVecValue );

    boost::scoped_ptr<xmlparser::tinyxml2::XmlParserImpl > parser(new xmlparser::tinyxml2::XmlParserImpl() );

    const char *xmlIntegerListFalse = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyIntegerList>\
                0, 1, -2, 3.1, -4.1;\
            </MyIntegerList>\
        </MyRoot>";

    parser->ReadFromMem(xmlIntegerListFalse);
    boost::shared_ptr<xmlparser::XmlParameter> param( parser->GetRoot() );

    std::vector<size_t> myIntegerVecFalse = param->GetElementUnsignedIntVecValue("MyIntegerList");

    TS_ASSERT_EQUALS(myIntegerVecFalse.size(),5);
    TS_ASSERT_EQUALS(myIntegerVecFalse.at(0),0);
    TS_ASSERT_EQUALS(myIntegerVecFalse.at(1),1);
    TS_ASSERT_EQUALS(myIntegerVecFalse.at(2),2);
    TS_ASSERT_EQUALS(myIntegerVecFalse.at(3),3);
    TS_ASSERT_EQUALS(myIntegerVecFalse.at(4),4);

    const char *xmlIntegerListThrow = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyIntegerList>\
                a;\
            </MyIntegerList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlIntegerListThrow);
    param =  parser->GetRoot();
    TS_ASSERT_THROWS_ASSERT( param->GetElementUnsignedIntVecValue("MyIntegerList")
        , const xmlparser::CouldNotReadElementContentException & e
        , TS_ASSERT_EQUALS( strcmp( e.what(), "The Element MyRoot has not the correct content:                 a;            . Line: 1." ),0 ) );
}

void TestParser::TestGetElementDoubleVecValue()
{
    SimpleTestcases<double>( &xmlparser::XmlParameter::GetElementDoubleVecValue);

    boost::scoped_ptr<xmlparser::tinyxml2::XmlParserImpl > parser(new xmlparser::tinyxml2::XmlParserImpl() );

    const char *xmlDoubleList2 = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                0, 1, -2, 3.1, -4.1;\
            </MyDoubleList>\
        </MyRoot>";

    parser->ReadFromMem(xmlDoubleList2);
    boost::shared_ptr<xmlparser::XmlParameter> param( parser->GetRoot() );
    std::vector<double> myDoubleVec2 = param->GetElementDoubleVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVec2.size(),5);
    TS_ASSERT_DELTA(myDoubleVec2.at(0),0.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVec2.at(1),1.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVec2.at(2),-2.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVec2.at(3),3.1, sDelta);
    TS_ASSERT_DELTA(myDoubleVec2.at(4),-4.1, sDelta);
}

void TestParser::TestGetElementDoubleVecVecValue()
{

    const char *xmlDoubleList = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                1.0, 2.0, 3.0, 4.0, 5.0;\
                6.0, 7.0, 8.0, 9.0, 10.0;\
            </MyDoubleList>\
        </MyRoot>";
    boost::scoped_ptr<xmlparser::tinyxml2::XmlParserImpl > parser(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlDoubleList);
    boost::shared_ptr<xmlparser::XmlParameter> param( parser->GetRoot() );
    std::vector<std::vector<double> > myDoubleVecVec = param->GetElementDoubleVecVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVecVec.size(),2);
    TS_ASSERT_DELTA(myDoubleVecVec.at(0).at(0),1.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(0).at(1),2.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(0).at(2),3.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(0).at(3),4.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(0).at(4),5.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(1).at(0),6.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(1).at(1),7.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(1).at(2),8.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(1).at(3),9.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec.at(1).at(4),10.0, sDelta);

    const char *xmlDoubleList1 = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                0, 1, -2, 3.1, -4.1;\
                6.0, 7.0, 8.0, 9.0, 10.0;\
            </MyDoubleList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlDoubleList1);
    param =  parser->GetRoot();
    std::vector<std::vector<double> > myDoubleVecVec1 = param->GetElementDoubleVecVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVecVec1.size(),2);
    TS_ASSERT_EQUALS(myDoubleVecVec1.at(0).size(),5);
    TS_ASSERT_EQUALS(myDoubleVecVec1.at(1).size(),5);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(0).at(0),0.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(0).at(1),1.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(0).at(2),-2.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(0).at(3),3.1, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(0).at(4),-4.1, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(1).at(0),6.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(1).at(1),7.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(1).at(2),8.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(1).at(3),9.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVec1.at(1).at(4),10.0, sDelta);

    const char *xmlDoubleListOneLine = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                0, 1, 2, 3;\
            </MyDoubleList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlDoubleListOneLine);
    param =  parser->GetRoot();
    std::vector<std::vector<double> > myDoubleVecVecOneLine = param->GetElementDoubleVecVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVecVecOneLine.size(),1);
    TS_ASSERT_DELTA(myDoubleVecVecOneLine.at(0).size(),4, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecOneLine.at(0).at(0),0.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecOneLine.at(0).at(1),1.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecOneLine.at(0).at(2),2.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecOneLine.at(0).at(3),3.0, sDelta);

    const char *xmlDoubleListShortNull = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                0;\
            </MyDoubleList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlDoubleListShortNull);
    param =  parser->GetRoot();
    std::vector<std::vector<double> > myDoubleVecVecShortNull = param->GetElementDoubleVecVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVecVecShortNull.size(),1);
    TS_ASSERT_DELTA(myDoubleVecVecShortNull.at(0).size(),1, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecShortNull.at(0).at(0),0.0, sDelta);

    const char *xmlDoubleListShort = "<?xml version='1.0'?>\
        <MyRoot>\
            <MyDoubleList>\
                5;\
                0;\
            </MyDoubleList>\
        </MyRoot>";
    parser.reset(new xmlparser::tinyxml2::XmlParserImpl() );
    parser->ReadFromMem(xmlDoubleListShort);
    param =  parser->GetRoot();
    std::vector<std::vector<double> > myDoubleVecVecShort = param->GetElementDoubleVecVecValue("MyDoubleList");

    TS_ASSERT_EQUALS(myDoubleVecVecShort.size(),2);
    TS_ASSERT_DELTA(myDoubleVecVecShort.at(0).size(),1, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecShort.at(1).size(),1, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecShort.at(0).at(0),5.0, sDelta);
    TS_ASSERT_DELTA(myDoubleVecVecShort.at(1).at(0),0.0, sDelta);
}
