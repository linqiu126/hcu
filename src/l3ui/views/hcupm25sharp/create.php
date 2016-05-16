<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25sharp */

$this->title = 'Create Hcupm25sharp';
$this->params['breadcrumbs'][] = ['label' => 'Hcupm25sharps', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcupm25sharp-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
