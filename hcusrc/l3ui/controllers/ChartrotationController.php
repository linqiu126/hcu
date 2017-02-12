<?php

namespace app\controllers;

use Yii;
use yii\web\Controller;
use yii\filters\AccessControl;

class ChartrotationController extends Controller
{
    //20150922注释，
    //先在头上加上use yii\filters\AccessControl;
    /*public function behaviors()
    {
        return [

            'access' => [
                'class' => AccessControl::className(),
                'only' => ['index', 'create', 'update', 'view'],
                'rules' => [
                    // allow authenticated users
                    [
                        'allow' => true,
                        'roles' => ['@'],
                    ],
                    // everything else is denied
                ],
            ],

        ];
    }*/
    //20150922注释，

    public function actionIndex()
    {
        return $this->render('index');
    }

}