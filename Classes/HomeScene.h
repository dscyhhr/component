//
//  HomeScene.h
//  ComponentTest
//
//  Created by 邓承 on 15/8/24.
//
//

#ifndef __ComponentTest__HomeScene__
#define __ComponentTest__HomeScene__

#include "cocos2d.h"

USING_NS_CC;

class HomeScene : public Scene
{
public:
    HomeScene();
    ~HomeScene();
    
    static HomeScene* create();
    virtual bool init();
    
protected://method
    
    void onEnter() override;
    
    void onExit() override;
    
private://Variables
}

#endif /* defined(__ComponentTest__HomeScene__) */
