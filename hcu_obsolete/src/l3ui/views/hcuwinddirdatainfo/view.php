<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcuwinddirdatainfo */

$this->title = $model->sid;
$this->params['breadcrumbs'][] = ['label' => 'Hcuwinddirdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuwinddirdatainfo-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->sid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->sid], [
            'class' => 'btn btn-danger',
            'data' => [
                'confirm' => 'Are you sure you want to delete this item?',
                'method' => 'post',
            ],
        ]) ?>
    </p>

    <?= DetailView::widget([
        'model' => $model,
        'attributes' => [
            'sid',
            'deviceid',
            'timestamp:datetime',
            'dataformat',
            'winddirvalue',
            'EW',
            'gpsx',
            'NS',
            'gpsy',
            'gpsz',
            'onofflineflag',
        ],
    ]) ?>

</div>
