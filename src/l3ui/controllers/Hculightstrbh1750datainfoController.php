<?php

namespace app\controllers;

use Yii;
use app\models\Hculightstrbh1750datainfo;
use app\models\Hculightstrbh1750datainfoSearch;
use yii\web\Controller;
use yii\web\NotFoundHttpException;
use yii\filters\VerbFilter;

/**
 * Hculightstrbh1750datainfoController implements the CRUD actions for Hculightstrbh1750datainfo model.
 */
class Hculightstrbh1750datainfoController extends Controller
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
     * Lists all Hculightstrbh1750datainfo models.
     * @return mixed
     */
    public function actionIndex()
    {
        $searchModel = new Hculightstrbh1750datainfoSearch();
        $dataProvider = $searchModel->search(Yii::$app->request->queryParams);

        return $this->render('index', [
            'searchModel' => $searchModel,
            'dataProvider' => $dataProvider,
        ]);
    }

    /**
     * Displays a single Hculightstrbh1750datainfo model.
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
     * Creates a new Hculightstrbh1750datainfo model.
     * If creation is successful, the browser will be redirected to the 'view' page.
     * @return mixed
     */
    public function actionCreate()
    {
        $model = new Hculightstrbh1750datainfo();

        if ($model->load(Yii::$app->request->post()) && $model->save()) {
            return $this->redirect(['view', 'id' => $model->sid]);
        } else {
            return $this->render('create', [
                'model' => $model,
            ]);
        }
    }

    /**
     * Updates an existing Hculightstrbh1750datainfo model.
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
     * Deletes an existing Hculightstrbh1750datainfo model.
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
     * Finds the Hculightstrbh1750datainfo model based on its primary key value.
     * If the model is not found, a 404 HTTP exception will be thrown.
     * @param integer $id
     * @return Hculightstrbh1750datainfo the loaded model
     * @throws NotFoundHttpException if the model cannot be found
     */
    protected function findModel($id)
    {
        if (($model = Hculightstrbh1750datainfo::findOne($id)) !== null) {
            return $model;
        } else {
            throw new NotFoundHttpException('The requested page does not exist.');
        }
    }

    public  function  actionGetlasthculightstrbh1750datainfo()
    //http://localhost/l3ui/web/index.php?r=hcuwindspddatainfo/getlasthcuwindspddatainfo&callback=myCallbackFunction    
    {
        
        \Yii::$app->response->format = \yii\web\Response::FORMAT_JSON; //直接使用的话，不用在web.php中定义JSONPARSE
        //\Yii::$app->response->format = \yii\web\Response::FORMAT_JSONP;
        //取最后一条记录，本地界面中，应该没有deviceid的考虑，应该是只连接一台风速传感器
        $Lasthculightstrbh1750datainfo = Hculightstrbh1750datainfo::find()->orderBy(['sid' => SORT_DESC])->one(); 

        /*print_r($Lasthcuwindspddatainfo);
        echo '<hr/>';*/

        $exportarray = $Lasthculightstrbh1750datainfo->attributes;
        //print_r($exportarray);
        return ['data' => $exportarray];
        //return ['callback' => $callback, 'data' => $exportarray];
    }
}
