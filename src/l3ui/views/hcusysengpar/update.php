<?php

use yii\helpers\Html;

/* @var $this yii\web\View */
/* @var $model app\models\Hcusysengpar */

$this->title = 'Update Hcusysengpar: ' . ' ' . $model->prjname;
$this->params['breadcrumbs'][] = ['label' => 'Hcusysengpars', 'url' => ['index']];
$this->params['breadcrumbs'][] = ['label' => $model->prjname, 'url' => ['view', 'id' => $model->prjname]];
$this->params['breadcrumbs'][] = 'Update';
?>
<div class="hcusysengpar-update">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
