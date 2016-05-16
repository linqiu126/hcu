<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcutracemodulectr */

$this->title = 'Create Hcutracemodulectr';
$this->params['breadcrumbs'][] = ['label' => 'Hcutracemodulectrs', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutracemodulectr-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
