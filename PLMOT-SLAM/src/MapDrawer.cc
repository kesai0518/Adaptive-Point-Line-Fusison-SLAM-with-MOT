/**
* This file is part of ORB-LINE-SLAM
*
* Copyright (C) 2020-2021 John Alamanos, National Technical University of Athens.
* Copyright (C) 2017-2020 Carlos Campos, Richard Elvira, Juan J. Gómez Rodríguez, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
* Copyright (C) 2014-2016 Raúl Mur-Artal, José M.M. Montiel and Juan D. Tardós, University of Zaragoza.
*
* ORB-LINE-SLAM is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-LINE-SLAM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
* the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with ORB-LINE-SLAM.
* If not, see <http://www.gnu.org/licenses/>.
*/


#include "MapDrawer.h"
#include "MapPoint.h"
#include "MapLine.h"
#include "KeyFrame.h"
#include <pangolin/pangolin.h>
#include <mutex>

namespace ORB_SLAM3
{


MapDrawer::MapDrawer(Atlas* pAtlas, const string &strSettingPath):mpAtlas(pAtlas)
{
    cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);

    bool is_correct = ParseViewerParamFile(fSettings);

    if(!is_correct)
    {
        std::cerr << "**ERROR in the config file, the format is not correct**" << std::endl;
        try
        {
            throw -1;
        }
        catch(exception &e)
        {

        }
    }
}

bool MapDrawer::ParseViewerParamFile(cv::FileStorage &fSettings)
{
    bool b_miss_params = false;

    cv::FileNode node = fSettings["Viewer.KeyFrameSize"];
    if(!node.empty())
    {
        mKeyFrameSize = node.real();
    }
    else
    {
        std::cerr << "*Viewer.KeyFrameSize parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    node = fSettings["Viewer.KeyFrameLineWidth"];
    if(!node.empty())
    {
        mKeyFrameLineWidth = node.real();
    }
    else
    {
        std::cerr << "*Viewer.KeyFrameLineWidth parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    node = fSettings["Viewer.GraphLineWidth"];
    if(!node.empty())
    {
        mGraphLineWidth = node.real();
    }
    else
    {
        std::cerr << "*Viewer.GraphLineWidth parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    node = fSettings["Viewer.PointSize"];
    if(!node.empty())
    {
        mPointSize = node.real();
    }
    else
    {
        std::cerr << "*Viewer.PointSize parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    node = fSettings["Viewer.CameraSize"];
    if(!node.empty())
    {
        mCameraSize = node.real();
    }
    else
    {
        std::cerr << "*Viewer.CameraSize parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    node = fSettings["Viewer.CameraLineWidth"];
    if(!node.empty())
    {
        mCameraLineWidth = node.real();
    }
    else
    {
        std::cerr << "*Viewer.CameraLineWidth parameter doesn't exist or is not a real number*" << std::endl;
        b_miss_params = true;
    }

    return !b_miss_params;
}

void MapDrawer::DrawMapPoints()
{
    const vector<MapPoint*> &vpMPs = mpAtlas->GetAllMapPoints();
    const vector<MapPoint*> &vpRefMPs = mpAtlas->GetReferenceMapPoints();
    const vector<cv::Mat> &vpCameraCenter = mpAtlas->GetAllCameraCenter();


    set<MapPoint*> spRefMPs(vpRefMPs.begin(), vpRefMPs.end());

    if(vpMPs.empty())
        return;

    // camera trajectory
    glColor3f(1.0,0.0,0.0);
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i<vpCameraCenter.size(); i++)
    {

        cv::Mat traj = vpCameraCenter[i];
        glVertex3f(traj.at<float>(0),traj.at<float>(1),traj.at<float>(2));

    }
    glEnd();

//    glPointSize(mPointSize);
    glPointSize(2.5);
    glBegin(GL_POINTS);
    glColor3f(1.0,0.0,0.0);

    for(size_t i=0, iend=vpMPs.size(); i<iend;i++)
    {
        if(vpMPs[i]->isBad() || spRefMPs.count(vpMPs[i]))
            continue;
        cv::Mat pos = vpMPs[i]->GetWorldPos();
        glVertex3f(pos.at<float>(0),pos.at<float>(1),pos.at<float>(2));
    }
    glEnd();

//    glPointSize(mPointSize);
    glPointSize(2.5);
    glBegin(GL_POINTS);
    glColor3f(1.0,0.0,0.0);

    for(set<MapPoint*>::iterator sit=spRefMPs.begin(), send=spRefMPs.end(); sit!=send; sit++)
    {
        if((*sit)->isBad())
            continue;
        cv::Mat pos = (*sit)->GetWorldPos();
        glVertex3f(pos.at<float>(0),pos.at<float>(1),pos.at<float>(2));

    }

    glEnd();
}

void MapDrawer::DrawMapLines()
{
    const vector<MapLine*> &vpMLs = mpAtlas->GetAllMapLines();
    const vector<MapLine*> &vpRefMLs = mpAtlas->GetReferenceMapLines();

    set<MapLine*> spRefMLs(vpRefMLs.begin(), vpRefMLs.end());

    if(vpMLs.empty())
        return;

    float mLineSize = 1.0;

    glLineWidth(mLineSize);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    for(size_t i=0, iend=vpMLs.size(); i<iend;i++)
    {
        if(vpMLs[i]->isBad() || spRefMLs.count(vpMLs[i]))
            continue;
        Vector6d sep = vpMLs[i]->GetWorldPos();
        Vector3d sp_eigen = sep.head(3);
        Vector3d ep_eigen = sep.tail(3);
        glVertex3f(static_cast<float>(sp_eigen(0)),static_cast<float>(sp_eigen(1)),static_cast<float>(sp_eigen(2)));
        glVertex3f(static_cast<float>(ep_eigen(0)),static_cast<float>(ep_eigen(1)),static_cast<float>(ep_eigen(2)));
    }
    glEnd();

    glPointSize(mLineSize);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
    for(set<MapLine*>::iterator sit=spRefMLs.begin(), send=spRefMLs.end(); sit!=send; sit++)
    {
        if((*sit)->isBad())
            continue;
        Vector6d sep = (*sit)->GetWorldPos();
        Vector3d sp_eigen = sep.head(3);
        Vector3d ep_eigen = sep.tail(3);
        glVertex3f(static_cast<float>(sp_eigen(0)),static_cast<float>(sp_eigen(1)),static_cast<float>(sp_eigen(2)));
        glVertex3f(static_cast<float>(ep_eigen(0)),static_cast<float>(ep_eigen(1)),static_cast<float>(ep_eigen(2)));

    }
    glEnd();
}

void MapDrawer::DrawMapDelaunayLines()
{
    const vector<cv::Mat> &vpMPs = mpAtlas->GetAllMapDelaunayLine();
    if(vpMPs.empty())
        return;

//    glBegin(GL_POINTS);
//    glLineWidth(1.5);

//    glPointSize(4);
//    glBegin(GL_POINTS);
//    glColor3f(1.0,0.0,1.0);

    glPointSize(4);
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_POINTS);
    for (size_t i=0, iend=vpMPs.size(); i<iend;i++)
    {

        cv::Mat pos = vpMPs[i];
        glVertex3f(pos.at<float>(0),pos.at<float>(1),pos.at<float>(2));
        glVertex3f(pos.at<float>(3),pos.at<float>(4),pos.at<float>(5));

    }
    glEnd();

    glLineWidth(1);
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
    for(size_t i=0, iend=vpMPs.size(); i<iend;i++)
    {
        cv::Mat pos = vpMPs[i];
        glVertex3f(pos.at<float>(0),pos.at<float>(1),pos.at<float>(2));
        glVertex3f(pos.at<float>(3),pos.at<float>(4),pos.at<float>(5));
    }
    glEnd();

}

void MapDrawer::DrawDynamicTrack()
{
    const vector<cv::Mat> &vpPerson3D = mpAtlas->GetPersonTrack();

    // 2023.06.11 for person tracking
    glPointSize(5);
    glColor3f(1.0,0.0,0.0);
    for (auto &vP3d : vpPerson3D)
    {
        glBegin(GL_POINTS);
        cv::Mat traj = vP3d;
        glVertex3f(traj.at<float>(0),traj.at<float>(1),traj.at<float>(2));

    }
    glEnd();

    glColor3f(0.0,1.0,1.0);
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i<vpPerson3D.size(); i++)
    {

        cv::Mat traj = vpPerson3D[i];
        glVertex3f(traj.at<float>(0),traj.at<float>(1),traj.at<float>(2));

    }
    glEnd();

}

void MapDrawer::DrawMapSemantic()
{
    const vector<pair<cv::Mat, vector<float>>> &vpSemanticMP = mpAtlas->GetAllSemanticMapPoints();
//    const vector<cv::Mat> &vpCameraCenter = mpAtlas->GetAllCameraCenter();
    const vector<pair<vector<float>, string>> &vpBound3D = mpAtlas->GetAllBound3D();

    // SemantucMapPoints
    glPointSize(4);
    for(auto &sMP : vpSemanticMP)
    {
        glBegin(GL_POINTS);
        cv::Mat pos = sMP.first;
        vector<float> color = sMP.second;
        glColor3f(color[0] / 255.0, color[1] / 255.0, color[2] / 255.0);
        glVertex3f(pos.at<float>(0),pos.at<float>(1),pos.at<float>(2));
    }
    glEnd();

    // camera trajectory
//    glColor3f(1.0,0.0,0.0);
//    glLineWidth(4);
//    glBegin(GL_LINE_STRIP);
//    for (int i = 0; i<vpCameraCenter.size(); i++)
//    {
//
//        cv::Mat traj = vpCameraCenter[i];
//        glVertex3f(traj.at<float>(0),traj.at<float>(1),traj.at<float>(2));
//
//    }
//    glEnd();

    // 3d Bounding Boxes
    //pangolin::GlFont *text_font = new pangolin::GlFont("/home/kesai/SLAM_ROS2/src/PointLineSLAM/Anonymous-Pro-Bold.ttf",20.0);
    glLineWidth(1.5);
    for (auto &vB3D : vpBound3D)
    {
        glColor3f(1.0,0.0,1.0);
        glBegin(GL_LINES);
        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[4]);
        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[4]);

        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[4]);
        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[5]);

        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[5]);
        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[5]);

        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[5]);
        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[4]);

        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[4]);
        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[4]);

        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[4]);
        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[5]);

        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[5]);
        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[5]);

        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[5]);
        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[4]);

        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[4]);
        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[4]);

        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[4]);
        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[4]);

        glVertex3f(vB3D.first[1],vB3D.first[2],vB3D.first[5]);
        glVertex3f(vB3D.first[1],vB3D.first[3],vB3D.first[5]);

        glVertex3f(vB3D.first[0],vB3D.first[2],vB3D.first[5]);
        glVertex3f(vB3D.first[0],vB3D.first[3],vB3D.first[5]);
        glEnd();

        glColor3f(0.0,0.0,1.0);
        string text = vB3D.second;
        mpAtlas->text_font->Text(text.c_str()).Draw(vB3D.first[0],vB3D.first[2],vB3D.first[4]);
        //text_font->Text(text.c_str()).Draw(vB3D.first[0],vB3D.first[2],vB3D.first[4]);

    }


}

void MapDrawer::DrawKeyFrames(const bool bDrawKF, const bool bDrawGraph, const bool bDrawInertialGraph)
{
    const float &w = mKeyFrameSize;
    const float h = w*0.75;
    const float z = w*0.6;

    const vector<KeyFrame*> vpKFs = mpAtlas->GetAllKeyFrames();

    if(bDrawKF)
    {
        for(size_t i=0; i<vpKFs.size(); i++)
        {
            KeyFrame* pKF = vpKFs[i];
            cv::Mat Twc = pKF->GetPoseInverse().t();
            unsigned int index_color = pKF->mnOriginMapId;

            glPushMatrix();

            glMultMatrixf(Twc.ptr<GLfloat>(0));

            if(!pKF->GetParent()) // It is the first KF in the map
            {
                glLineWidth(mKeyFrameLineWidth*5);
                glColor3f(1.0f,0.0f,0.0f);
                glBegin(GL_LINES);

                //cout << "Initial KF: " << mpAtlas->GetCurrentMap()->GetOriginKF()->mnId << endl;
                //cout << "Parent KF: " << vpKFs[i]->mnId << endl;
            }
            else
            {
                glLineWidth(mKeyFrameLineWidth);
                //glColor3f(0.0f,0.0f,1.0f);
                glColor3f(mfFrameColors[index_color][0],mfFrameColors[index_color][1],mfFrameColors[index_color][2]);
                glBegin(GL_LINES);
            }

            glVertex3f(0,0,0);
            glVertex3f(w,h,z);
            glVertex3f(0,0,0);
            glVertex3f(w,-h,z);
            glVertex3f(0,0,0);
            glVertex3f(-w,-h,z);
            glVertex3f(0,0,0);
            glVertex3f(-w,h,z);

            glVertex3f(w,h,z);
            glVertex3f(w,-h,z);

            glVertex3f(-w,h,z);
            glVertex3f(-w,-h,z);

            glVertex3f(-w,h,z);
            glVertex3f(w,h,z);

            glVertex3f(-w,-h,z);
            glVertex3f(w,-h,z);
            glEnd();

            glPopMatrix();

            //Draw lines with Loop and Merge candidates
            /*glLineWidth(mGraphLineWidth);
            glColor4f(1.0f,0.6f,0.0f,1.0f);
            glBegin(GL_LINES);
            cv::Mat Ow = pKF->GetCameraCenter();
            const vector<KeyFrame*> vpLoopCandKFs = pKF->mvpLoopCandKFs;
            if(!vpLoopCandKFs.empty())
            {
                for(vector<KeyFrame*>::const_iterator vit=vpLoopCandKFs.begin(), vend=vpLoopCandKFs.end(); vit!=vend; vit++)
                {
                    cv::Mat Ow2 = (*vit)->GetCameraCenter();
                    glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                    glVertex3f(Ow2.at<float>(0),Ow2.at<float>(1),Ow2.at<float>(2));
                }
            }
            const vector<KeyFrame*> vpMergeCandKFs = pKF->mvpMergeCandKFs;
            if(!vpMergeCandKFs.empty())
            {
                for(vector<KeyFrame*>::const_iterator vit=vpMergeCandKFs.begin(), vend=vpMergeCandKFs.end(); vit!=vend; vit++)
                {
                    cv::Mat Ow2 = (*vit)->GetCameraCenter();
                    glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                    glVertex3f(Ow2.at<float>(0),Ow2.at<float>(1),Ow2.at<float>(2));
                }
            }*/

            glEnd();
        }
    }

    if(bDrawGraph)
    {
        glLineWidth(mGraphLineWidth);
        glColor4f(0.0f,1.0f,0.0f,0.6f);
        glBegin(GL_LINES);

        // cout << "-----------------Draw graph-----------------" << endl;
        for(size_t i=0; i<vpKFs.size(); i++)
        {
            // Covisibility Graph
            const vector<KeyFrame*> vCovKFs = vpKFs[i]->GetCovisiblesByWeight(100);
            cv::Mat Ow = vpKFs[i]->GetCameraCenter();
            if(!vCovKFs.empty())
            {
                for(vector<KeyFrame*>::const_iterator vit=vCovKFs.begin(), vend=vCovKFs.end(); vit!=vend; vit++)
                {
                    if((*vit)->mnId<vpKFs[i]->mnId)
                        continue;
                    cv::Mat Ow2 = (*vit)->GetCameraCenter();
                    glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                    glVertex3f(Ow2.at<float>(0),Ow2.at<float>(1),Ow2.at<float>(2));
                }
            }

            // Spanning tree
            KeyFrame* pParent = vpKFs[i]->GetParent();
            if(pParent)
            {
                cv::Mat Owp = pParent->GetCameraCenter();
                glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                glVertex3f(Owp.at<float>(0),Owp.at<float>(1),Owp.at<float>(2));
            }

            // Loops
            set<KeyFrame*> sLoopKFs = vpKFs[i]->GetLoopEdges();
            for(set<KeyFrame*>::iterator sit=sLoopKFs.begin(), send=sLoopKFs.end(); sit!=send; sit++)
            {
                if((*sit)->mnId<vpKFs[i]->mnId)
                    continue;
                cv::Mat Owl = (*sit)->GetCameraCenter();
                glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                glVertex3f(Owl.at<float>(0),Owl.at<float>(1),Owl.at<float>(2));
            }
        }

        glEnd();
    }

    if(bDrawInertialGraph && mpAtlas->isImuInitialized())
    {
        glLineWidth(mGraphLineWidth);
        glColor4f(1.0f,0.0f,0.0f,0.6f);
        glBegin(GL_LINES);

        //Draw inertial links
        for(size_t i=0; i<vpKFs.size(); i++)
        {
            KeyFrame* pKFi = vpKFs[i];
            cv::Mat Ow = pKFi->GetCameraCenter();
            KeyFrame* pNext = pKFi->mNextKF;
            if(pNext)
            {
                cv::Mat Owp = pNext->GetCameraCenter();
                glVertex3f(Ow.at<float>(0),Ow.at<float>(1),Ow.at<float>(2));
                glVertex3f(Owp.at<float>(0),Owp.at<float>(1),Owp.at<float>(2));
            }
        }

        glEnd();
    }

    vector<Map*> vpMaps = mpAtlas->GetAllMaps();

    if(bDrawKF)
    {
        for(Map* pMap : vpMaps)
        {
            if(pMap == mpAtlas->GetCurrentMap())
                continue;

            vector<KeyFrame*> vpKFs = pMap->GetAllKeyFrames();

            for(size_t i=0; i<vpKFs.size(); i++)
            {
                KeyFrame* pKF = vpKFs[i];
                cv::Mat Twc = pKF->GetPoseInverse().t();
                unsigned int index_color = pKF->mnOriginMapId;

                glPushMatrix();

                glMultMatrixf(Twc.ptr<GLfloat>(0));

                if(!vpKFs[i]->GetParent()) // It is the first KF in the map
                {
                    glLineWidth(mKeyFrameLineWidth*5);
                    glColor3f(1.0f,0.0f,0.0f);
                    glBegin(GL_LINES);
                }
                else
                {
                    glLineWidth(mKeyFrameLineWidth);
                    //glColor3f(0.0f,0.0f,1.0f);
                    glColor3f(mfFrameColors[index_color][0],mfFrameColors[index_color][1],mfFrameColors[index_color][2]);
                    glBegin(GL_LINES);
                }

                glVertex3f(0,0,0);
                glVertex3f(w,h,z);
                glVertex3f(0,0,0);
                glVertex3f(w,-h,z);
                glVertex3f(0,0,0);
                glVertex3f(-w,-h,z);
                glVertex3f(0,0,0);
                glVertex3f(-w,h,z);

                glVertex3f(w,h,z);
                glVertex3f(w,-h,z);

                glVertex3f(-w,h,z);
                glVertex3f(-w,-h,z);

                glVertex3f(-w,h,z);
                glVertex3f(w,h,z);

                glVertex3f(-w,-h,z);
                glVertex3f(w,-h,z);
                glEnd();

                glPopMatrix();
            }
        }
    }
}

void MapDrawer::DrawCurrentCamera(pangolin::OpenGlMatrix &Twc)
{
    const float &w = mCameraSize;
    const float h = w*0.75;
    const float z = w*0.6;

    glPushMatrix();

#ifdef HAVE_GLES
        glMultMatrixf(Twc.m);
#else
        glMultMatrixd(Twc.m);
#endif

    glLineWidth(mCameraLineWidth);
    glColor3f(0.0f,1.0f,0.0f);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(w,h,z);
    glVertex3f(0,0,0);
    glVertex3f(w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,h,z);

    glVertex3f(w,h,z);
    glVertex3f(w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(-w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(w,h,z);

    glVertex3f(-w,-h,z);
    glVertex3f(w,-h,z);
    glEnd();

    glPopMatrix();
}


void MapDrawer::SetCurrentCameraPose(const cv::Mat &Tcw)
{
    unique_lock<mutex> lock(mMutexCamera);
    mCameraPose = Tcw.clone();
}

void MapDrawer::GetCurrentOpenGLCameraMatrix(pangolin::OpenGlMatrix &M, pangolin::OpenGlMatrix &MOw)
{
    if(!mCameraPose.empty())
    {
        cv::Mat Rwc(3,3,CV_32F);
        cv::Mat twc(3,1,CV_32F);
        {
            unique_lock<mutex> lock(mMutexCamera);
            Rwc = mCameraPose.rowRange(0,3).colRange(0,3).t();
            twc = -Rwc*mCameraPose.rowRange(0,3).col(3);
        }

        M.m[0] = Rwc.at<float>(0,0);
        M.m[1] = Rwc.at<float>(1,0);
        M.m[2] = Rwc.at<float>(2,0);
        M.m[3]  = 0.0;

        M.m[4] = Rwc.at<float>(0,1);
        M.m[5] = Rwc.at<float>(1,1);
        M.m[6] = Rwc.at<float>(2,1);
        M.m[7]  = 0.0;

        M.m[8] = Rwc.at<float>(0,2);
        M.m[9] = Rwc.at<float>(1,2);
        M.m[10] = Rwc.at<float>(2,2);
        M.m[11]  = 0.0;

        M.m[12] = twc.at<float>(0);
        M.m[13] = twc.at<float>(1);
        M.m[14] = twc.at<float>(2);
        M.m[15]  = 1.0;

        MOw.SetIdentity();
        MOw.m[12] = twc.at<float>(0);
        MOw.m[13] = twc.at<float>(1);
        MOw.m[14] = twc.at<float>(2);
    }
    else
    {
        M.SetIdentity();
        MOw.SetIdentity();
    }
}

void MapDrawer::GetCurrentOpenGLCameraMatrix(pangolin::OpenGlMatrix &M, pangolin::OpenGlMatrix &MOw, pangolin::OpenGlMatrix &MTwwp)
{
    if(!mCameraPose.empty())
    {
        cv::Mat Rwc(3,3,CV_32F);
        cv::Mat twc(3,1,CV_32F);
        cv::Mat Rwwp(3,3,CV_32F);
        {
            unique_lock<mutex> lock(mMutexCamera);
            Rwc = mCameraPose.rowRange(0,3).colRange(0,3).t();
            twc = -Rwc*mCameraPose.rowRange(0,3).col(3);
        }

        M.m[0] = Rwc.at<float>(0,0);
        M.m[1] = Rwc.at<float>(1,0);
        M.m[2] = Rwc.at<float>(2,0);
        M.m[3]  = 0.0;

        M.m[4] = Rwc.at<float>(0,1);
        M.m[5] = Rwc.at<float>(1,1);
        M.m[6] = Rwc.at<float>(2,1);
        M.m[7]  = 0.0;

        M.m[8] = Rwc.at<float>(0,2);
        M.m[9] = Rwc.at<float>(1,2);
        M.m[10] = Rwc.at<float>(2,2);
        M.m[11]  = 0.0;

        M.m[12] = twc.at<float>(0);
        M.m[13] = twc.at<float>(1);
        M.m[14] = twc.at<float>(2);
        M.m[15]  = 1.0;

        MOw.SetIdentity();
        MOw.m[12] = twc.at<float>(0);
        MOw.m[13] = twc.at<float>(1);
        MOw.m[14] = twc.at<float>(2);

        MTwwp.SetIdentity();
        MTwwp.m[0] = Rwwp.at<float>(0,0);
        MTwwp.m[1] = Rwwp.at<float>(1,0);
        MTwwp.m[2] = Rwwp.at<float>(2,0);

        MTwwp.m[4] = Rwwp.at<float>(0,1);
        MTwwp.m[5] = Rwwp.at<float>(1,1);
        MTwwp.m[6] = Rwwp.at<float>(2,1);

        MTwwp.m[8] = Rwwp.at<float>(0,2);
        MTwwp.m[9] = Rwwp.at<float>(1,2);
        MTwwp.m[10] = Rwwp.at<float>(2,2);

        MTwwp.m[12] = twc.at<float>(0);
        MTwwp.m[13] = twc.at<float>(1);
        MTwwp.m[14] = twc.at<float>(2);
    }
    else
    {
        M.SetIdentity();
        MOw.SetIdentity();
        MTwwp.SetIdentity();
    }

}

} //namespace ORB_SLAM
