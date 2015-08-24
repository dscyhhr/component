//
//  HomeScene.cpp
//  ComponentTest
//
//  Created by 邓承 on 15/8/24.
//
//

#include "HomeScene.h"

HomeScene::HomeScene()
{
    
}

void HomeScene::onEnter()
{
    Scene::onEnter();
    
}

HomeScene* HomeScene::create()
{
    auto pRet = new HomeScene();
    if(pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    pRet = nullptr;
    return nullptr;
}

bool HomeScene::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    return true;
}

void HomeScene::onExit()
{
    
    Scene::onExit();
}
HomeScene::~HomeScene()
{
    
}