<?php

use yii\helpers\Html;
use yii\widgets\DetailView;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemodulectr */

$this->title = $model->moduleid;
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemodulectrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutracemodulectr-view">

    <h1><?= Html::encode($this->title) ?></h1>

    <p>
        <?= Html::a('Update', ['update', 'id' => $model->moduleid], ['class' => 'btn btn-primary']) ?>
        <?= Html::a('Delete', ['delete', 'id' => $model->moduleid], [
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
            'moduleid',
            'modulename',
            'modulectrflag',
            'moduletoallow',
            'moduletorestrict',
            'modulefromallow',
            'modulefromrestrict',
        ],
    ]) ?>

</div>
