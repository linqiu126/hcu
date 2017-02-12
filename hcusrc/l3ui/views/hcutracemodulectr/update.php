<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemodulectr */

$this->title = 'Update Hcutracemodulectr: ' . ' ' . $model->moduleid;
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemodulectrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->moduleid, 'url' => ['view', 'id' => $model->moduleid]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcutracemodulectr-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
