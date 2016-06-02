<?php

namespace app\controllers;

use Yii;
use app\models\Hcutoxicgasmq135datainfo;
use app\models\Hcutoxicgasmq135datainfoSearch;
use yii\web\Controller;
use yii\web\NotFoundHttpException;
use yii\filters\VerbFilter;

/**
 * Hcutoxicgasmq135datainfoController implements the CRUD actions for Hcutoxicgasmq135datainfo model.
 */
class Hcutoxicgasmq135datainfoController extends Controller
{
    public function behaviors()
    {
        return [
            'verbs' => [
                'class' => VerbFilter::className(),
                'actions' => [
                    'delete' => ['POST'],
                ],
            ],
        ];
    }

    /**
     * Lists all Hcutoxicgasmq135datainfo models.
     * @return mixed
     */
    public function actionIndex()
    {
        $searchModel = new Hcutoxicgasmq135datainfoSearch();
        $dataProvider = $searchModel->search(Yii::$app->request->queryParams);

        return $this->render('index', [
            'searchModel' => $searchModel,
            'dataProvider' => $dataProvider,
        ]);
    }

    /**
     * Displays a single Hcutoxicgasmq135datainfo model.
     * @param integer $id
     * @return mixed
     */
    public function actionView($id)
    {
        return $this->render('view', [
            'model' => $this->findModel($id),
        ]);
    }

    /**
     * Creates a new Hcutoxicgasmq135datainfo model.
     * If creation is successful, the browser will be redirected to the 'view' page.
     * @return mixed
     */
    public function actionCreate()
    {
        $model = new Hcutoxicgasmq135datainfo();

        if ($model->load(Yii::$app->request->post()) && $model->save()) {
            return $this->redirect(['view', 'id' => $model->sid]);
        } else {
            return $this->render('create', [
                'model' => $model,
            ]);
        }
    }

    /**
     * Updates an existing Hcutoxicgasmq135datainfo model.
     * If update is successful, the browser will be redirected to the 'view' page.
     * @param integer $id
     * @return mixed
     */
    public function actionUpdate($id)
    {
        $model = $this->findModel($id);

        if ($model->load(Yii::$app->request->post()) && $model->save()) {
            return $this->redirect(['view', 'id' => $model->sid]);
        } else {
            return $this->render('update', [
                'model' => $model,
            ]);
        }
    }

    /**
     * Deletes an existing Hcutoxicgasmq135datainfo model.
     * If deletion is successful, the browser will be redirected to the 'index' page.
     * @param integer $id
     * @return mixed
     */
    public function actionDelete($id)
    {
        $this->findModel($id)->delete();

        return $this->redirect(['index']);
    }

    /**
     * Finds the Hcutoxicgasmq135datainfo model based on its primary key value.
     * If the model is not found, a 404 HTTP exception will be thrown.
     * @param integer $id
     * @return Hcutoxicgasmq135datainfo the loaded model
     * @throws NotFoundHttpException if the model cannot be found
     */
    protected function findModel($id)
    {
        if (($model = Hcutoxicgasmq135datainfo::findOne($id)) !== null) {
            return $model;
        } else {
            throw new NotFoundHttpException('The requested page does not exist.');
        }
    }

    public  function  actionGetlasthcutoxicgasmq135datainfo()
    //http://localhost/l3ui/web/index.php?r=hcuwindspddatainfo/getlasthcuwindspddatainfo&callback=myCallbackFunction    
    {
        
        \Yii::$app->response->format = \yii\web\Response::FORMAT_JSON; //直接使用的话，不用在web.php中定义JSONPARSE
        //\Yii::$app->response->format = \yii\web\Response::FORMAT_JSONP;
        //取最后一条记录，本地界面中，应该没有deviceid的考虑，应该是只连接一台风速传感器
        $Lasthcutoxicgasmq135datainfo = Hcutoxicgasmq135datainfo::find()->orderBy(['sid' => SORT_DESC])->one(); 

        /*print_r($Lasthcuwindspddatainfo);
        echo '<hr/>';*/

        $exportarray = $Lasthcutoxicgasmq135datainfo->attributes;
        //print_r($exportarray);
        return ['data' => $exportarray];
        //return ['callback' => $callback, 'data' => $exportarray];
    }
}
