<?php

use yii\helpers\Html;
use yii\grid\GridView;

/* @var $this yii\web\View */
/* @var $searchModel app\models\HcutracemsgctrSearch */
/* @var $dataProvider yii\data\ActiveDataProvider */

$this->title = 'Hcutracemsgctrs';
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutracemsgctr-index">

    <h1><?= Html::encode($this->title) ?></h1>
    <?php // echo $this->render('_search', ['model' => $searchModel]); ?>

    <p>
        <?= Html::a('Create Hcutracemsgctr', ['create'], ['class' => 'btn btn-success']) ?>
    </p>

    <?= GridView::widget([
        'dataProvider' => $dataProvider,
        'filterModel' => $searchModel,
        'columns' => [
            ['class' => 'yii\grid\SerialColumn'],

            'msgid',
            'msgname',
            'msgctrflag',
            'msgallow',
            'msgrestrict',

            ['class' => 'yii\grid\ActionColumn'],
        ],
    ]); ?>

</div>
