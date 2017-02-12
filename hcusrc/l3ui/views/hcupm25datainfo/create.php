<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcupm25datainfo */

$this->title = 'Create Hcupm25datainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcupm25datainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcupm25datainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
