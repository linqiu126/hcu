<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcuemcdatainfo */

$this->title = 'Create Hcuemcdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcuemcdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcuemcdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
